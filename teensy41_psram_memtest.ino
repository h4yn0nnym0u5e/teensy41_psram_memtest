/*
   Note that this test fails with ISSI 16MByte parts if prefetch is on
   using https://github.com/PaulStoffregen/cores/pull/708 code
   
   Speed  prefetch Duration
   AP6404 8+8MB
   105.6    on       61.66 (15.1% faster)
    88.0    on       70.11
   105.6    off      72.65
    88.0    off      82.41 ==

   ISSI 16MB
   105.6    off      72.52
    88.0    off      82.39 ==
    49.5    off     125.24
    
   With BUFSZ(8) - test passes
   105.6    on       67.52 (6.9% faster) 

   Original test passes using 16MB ISSI PSRAM
   (note different algorithm, so duration NOT comparable)
   Speed  prefetch Duration (16MB)
   105.6    on       48.19

 105.6MHz is 20% faster than 88MHz, but the test only runs ~12% faster,
 so test overhead is slightly masking the speed increase.

 The combined clock and "safe" pre-fetch changes give 16.7% speed-up,
 but it appears that using the "unsafe" pre-fetch gives 23.8%, so
 it may well be worthwhile selecting the pre-fetch scheme at boot time
 depending on the parts detected.
*/
extern "C" uint8_t external_psram_size;

bool memory_ok = false;
uint32_t *memory_begin, *memory_end;

bool check_fixed_pattern(uint32_t pattern);
bool check_lfsr_pattern(uint32_t seed);

void setup()
{
    while (!Serial) ; // wait
    pinMode(13, OUTPUT);
    
    uint8_t size = external_psram_size, size1 = FLEXSPI2_FLSHA1CR0 >> 10;
    Serial.printf("EXTMEM Memory Test, %d MByte (%d+%d)\n", size, size1, size - size1);
    if (size == 0) return;
    
    const float clocks[4] = {396.0f, 720.0f, 664.62f, 528.0f};
    const float frequency = clocks[(CCM_CBCMR >> 8) & 3] / (float)(((CCM_CBCMR >> 29) & 7) + 1);
    Serial.printf(" CCM_CBCMR=%08X (%.1f MHz)\n", CCM_CBCMR, frequency);
    Serial.printf(" Pre-fetch is %sabled\n", (FLEXSPI2_AHBCR & FLEXSPI_AHBCR_PREFETCHEN) ? "en" : "dis");
    
    memory_begin = (uint32_t *)(0x7000'0000);
    memory_end = (uint32_t *)(0x7000'0000 + size * 1'048'576);
    elapsedMillis msec = 0;
    
    if (!check_fixed_pattern(0x5A698421)) return;
    if (!check_lfsr_pattern(2976674124ul)) return;
    if (!check_lfsr_pattern(1438200953ul)) return;
    if (!check_lfsr_pattern(3413783263ul)) return;
    if (!check_lfsr_pattern(1900517911ul)) return;
    if (!check_lfsr_pattern(1227909400ul)) return;
    if (!check_lfsr_pattern(276562754ul)) return;
    if (!check_lfsr_pattern(146878114ul)) return;
    if (!check_lfsr_pattern(615545407ul)) return;
    if (!check_lfsr_pattern(110497896ul)) return;
    if (!check_lfsr_pattern(74539250ul)) return;
    if (!check_lfsr_pattern(4197336575ul)) return;
    if (!check_lfsr_pattern(2280382233ul)) return;
    if (!check_lfsr_pattern(542894183ul)) return;
    if (!check_lfsr_pattern(3978544245ul)) return;
    if (!check_lfsr_pattern(2315909796ul)) return;
    if (!check_lfsr_pattern(3736286001ul)) return;
    if (!check_lfsr_pattern(2876690683ul)) return;
    if (!check_lfsr_pattern(215559886ul)) return;
    if (!check_lfsr_pattern(539179291ul)) return;
    if (!check_lfsr_pattern(537678650ul)) return;
    if (!check_lfsr_pattern(4001405270ul)) return;
    if (!check_lfsr_pattern(2169216599ul)) return;
    if (!check_lfsr_pattern(4036891097ul)) return;
    if (!check_lfsr_pattern(1535452389ul)) return;
    if (!check_lfsr_pattern(2959727213ul)) return;
    if (!check_lfsr_pattern(4219363395ul)) return;
    if (!check_lfsr_pattern(1036929753ul)) return;
    if (!check_lfsr_pattern(2125248865ul)) return;
    if (!check_lfsr_pattern(3177905864ul)) return;
    if (!check_lfsr_pattern(2399307098ul)) return;
    if (!check_lfsr_pattern(3847634607ul)) return;
    if (!check_lfsr_pattern(27467969ul)) return;
    if (!check_lfsr_pattern(520563506ul)) return;
    if (!check_lfsr_pattern(381313790ul)) return;
    if (!check_lfsr_pattern(4174769276ul)) return;
    if (!check_lfsr_pattern(3932189449ul)) return;
    if (!check_lfsr_pattern(4079717394ul)) return;
    if (!check_lfsr_pattern(868357076ul)) return;
    if (!check_lfsr_pattern(2474062993ul)) return;
    if (!check_lfsr_pattern(1502682190ul)) return;
    if (!check_lfsr_pattern(2471230478ul)) return;
    if (!check_lfsr_pattern(85016565ul)) return;
    if (!check_lfsr_pattern(1427530695ul)) return;
    if (!check_lfsr_pattern(1100533073ul)) return;
    if (!check_fixed_pattern(0x55555555)) return;
    if (!check_fixed_pattern(0x33333333)) return;
    if (!check_fixed_pattern(0x0F0F0F0F)) return;
    if (!check_fixed_pattern(0x00FF00FF)) return;
    if (!check_fixed_pattern(0x0000FFFF)) return;
    if (!check_fixed_pattern(0xAAAAAAAA)) return;
    if (!check_fixed_pattern(0xCCCCCCCC)) return;
    if (!check_fixed_pattern(0xF0F0F0F0)) return;
    if (!check_fixed_pattern(0xFF00FF00)) return;
    if (!check_fixed_pattern(0xFFFF0000)) return;
    if (!check_fixed_pattern(0xFFFFFFFF)) return;
    if (!check_fixed_pattern(0x00000000)) return;
    Serial.printf(" test ran for %.2f seconds\n", (float)msec / 1000.0f);
    Serial.println("All memory tests passed :-)");
    memory_ok = true;
}


///////////////////////////////////////////////////////////////////
// Use memcpy() etc. to do fast reads from / writes to PSRAM,
// with a length that will often cross any page boundary, i.e.
// avoiding the typical multiples of 32 or 1024 bytes. If there's
// an issue, the page start may be corrupted by the end of a write,
// and get picked up by the subsequent read. This won't of course
// cause an issue with the fixed values...
///////////////////////////////////////////////////////////////////
uint32_t reg;

#define BLK_SIZE 255 // 255*uint32_t is 1020 bytes
uint32_t regMulti[BLK_SIZE];

bool new_fail_message(uint32_t* pm, volatile uint32_t *location, int count)
{
  //Serial.printf(" Error at %08X, read %08X but expected %08X\n",
  //  (uint32_t)location, actual, expected);
  Serial.printf("Error at %08X\n",
                (uint32_t)location);
  int n = 16;
  uint32_t* pr = regMulti;
  //uint32_t* pm = location;
  while (count > 0)
  {
    Serial.printf("%08X: ", (uint32_t) location);
    for (int i = 0; i < n; i++) Serial.printf("%08X ", pr[i]);
    Serial.print("\n          ");
    for (int i = 0; i < n; i++) Serial.printf("%08X ", pm[i]);
    Serial.print("\n          ");
    for (int i = 0; i < n; i++) Serial.printf("%s ", pm[i] == pr[i] ? "        " : "^^^^^^^^");
    Serial.println();
    count -= n;
    location += n;
    pr += n;
    pm += n;

    if (count < n)
      n = count;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// fill the entire RAM with a fixed pattern, then check it
///////////////////////////////////////////////////////////////////
void nextRegFixed(uint32_t pattern)
{
  for (int i = 0; i < BLK_SIZE; i++) regMulti[i] = pattern;
}


bool check_fixed_pattern(uint32_t pattern)
{
  volatile uint32_t *p;
  Serial.printf("testing with fixed pattern %08X\n", pattern);

  p = memory_begin;
  nextRegFixed(pattern); // do once, value is fixed

  while (p < memory_end)
  {
    if (memory_end - p > BLK_SIZE)
    {
      memcpy((void*) p, regMulti, sizeof regMulti);
      p += sizeof regMulti / sizeof * p;
    }
    else
    {
      int count = memory_end - p;
      memcpy((void*) p, regMulti, count * sizeof * p);
      p += count;
    }
  }

  arm_dcache_flush_delete((void *)memory_begin,
                          (uint32_t)memory_end - (uint32_t)memory_begin);

  p = memory_begin;
  while (p < memory_end)
  {
    int cmpres = 999;
    uint32_t memBuff[BLK_SIZE];
    int count = memory_end - p;

    if (count > BLK_SIZE)
    {
      memcpy(memBuff, (void*) p, sizeof memBuff);
      cmpres = memcmp(memBuff, regMulti, sizeof regMulti);
      p += sizeof regMulti / sizeof * p;
      count = BLK_SIZE;
    }
    else
    {
      memcpy(memBuff, (void*) p, count * sizeof * p);
      cmpres = memcmp(memBuff, regMulti, count * sizeof * p);
      p += count;
    }
    if (0 != cmpres) return new_fail_message(memBuff, p - count, count);
    //Serial.printf(" reg=%08X\n", reg);
  }

  return true;
}


///////////////////////////////////////////////////////////////////
// fill the entire RAM with a pseudo-random sequence, then check it
///////////////////////////////////////////////////////////////////
uint32_t nextReg(void)
{
  uint32_t retval = reg;
  for (int i = 0; i < 3; i++) {
    // https://en.wikipedia.org/wiki/Xorshift
    reg ^= reg << 13;
    reg ^= reg >> 17;
    reg ^= reg << 5;
  }
  return retval;
}


void nextRegMulti(void)
{
  for (int i = 0; i < BLK_SIZE; i++)
    regMulti[i] = nextReg();
}


bool check_lfsr_pattern(uint32_t seed)
{
  volatile uint32_t *p;

  Serial.printf("testing with pseudo-random sequence, seed=%u\n", seed);
  reg = seed;
  p = memory_begin;
  while (p < memory_end)
  {
    nextRegMulti();
    if (memory_end - p > BLK_SIZE)
    {
      memcpy((void*) p, regMulti, sizeof regMulti);
      p += sizeof regMulti / sizeof * p;
    }
    else
    {
      int count = memory_end - p;
      memcpy((void*) p, regMulti, count * sizeof *p);
      p += count;
    }
  }

  arm_dcache_flush_delete((void *)memory_begin,
                          (uint32_t)memory_end - (uint32_t)memory_begin);

  reg = seed;
  p = memory_begin;
  while (p < memory_end)
  {
    int cmpres = 999;
    uint32_t memBuff[BLK_SIZE];
    int count = memory_end - p;

    nextRegMulti();
    if (count >= BLK_SIZE)
    {
      count = BLK_SIZE;
      const int sz = count * sizeof *p;
      memcpy(memBuff, (void*) p, sz);
      cmpres = memcmp(memBuff, regMulti, sz);
    }
    else
    {
      const int sz = count * sizeof *p;
      memcpy(memBuff, (void*) p, sz);
      cmpres = memcmp(memBuff, regMulti, sz);
    }
    p += count;
    if (0 != cmpres) return new_fail_message(memBuff, p - count, count);
    //Serial.printf(" reg=%08X\n", reg);
  }
  return true;
}


void loop()
{
  digitalWrite(13, HIGH);
  delay(100);
  if (!memory_ok) digitalWrite(13, LOW); // rapid blink if any test fails
  delay(100);
}
