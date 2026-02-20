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
#include <Arduino.h>
#include <DMAChannel.h>

extern "C" uint8_t external_psram_size;

bool memory_ok = false;
uint32_t *memory_begin, *memory_end;
int test_count;

#define SOME_TESTS_USE_DMA true
bool check_fixed_pattern(uint32_t pattern, bool useDMA = false);
bool check_lfsr_pattern(uint32_t seed, bool useDMA = false);


void printPrefetchSetting(uint32_t setting)
{
  if (setting & FLEXSPI_AHBRXBUFCR0_PREFETCHEN)
  {
    const char* master="????";
    switch ((setting & FLEXSPI_AHBRXBUFCR0_MSTRID_MASK) / FLEXSPI_AHBRXBUFCR0_MSTRID(1))
    {
      case 0: master = " CPU"; break;
      case 1: master = "eDMA"; break;
      case 2: master = " DCP"; break;
      default: break;
    }
    Serial.printf("  %s: BUFSZ=%d\n", master, FLEXSPI_AHBRXBUFCR0_BUFSZ(setting));
  }
}


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

    bool prefetch = 0 != (FLEXSPI2_AHBCR & FLEXSPI_AHBCR_PREFETCHEN);
    Serial.printf(" Pre-fetch is %sabled\n", prefetch ? "en" : "dis");
    if (prefetch && 0 != FLEXSPI2_AHBRXBUF0CR0)
    {
      printPrefetchSetting(FLEXSPI2_AHBRXBUF0CR0);
      printPrefetchSetting(FLEXSPI2_AHBRXBUF1CR0);
      printPrefetchSetting(FLEXSPI2_AHBRXBUF2CR0);
      printPrefetchSetting(FLEXSPI2_AHBRXBUF3CR0);
    }
    Serial.printf("%s tests use DMA memory copying",SOME_TESTS_USE_DMA?"Some":"No");

    Serial.println();
    
    memory_begin = (uint32_t *)(0x7000'0000);
    memory_end = (uint32_t *)(0x7000'0000 + size * 1'048'576);
    elapsedMillis msec = 0;
    
    if (!check_fixed_pattern(0x5A698421)) return;
    if (!check_lfsr_pattern(2976674124ul)) return;
    if (!check_lfsr_pattern(1438200953ul, SOME_TESTS_USE_DMA)) return;
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
    if (!check_fixed_pattern(0x55555555, SOME_TESTS_USE_DMA)) return;
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
    Serial.printf(" %d tests took %.2f seconds\n", test_count, (float)msec / 1000.0f);
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

//#define BLK_SIZE 255 // 255*uint32_t is 1020 bytes
#define BLK_SIZE 511 // 511*uint32_t is 2044 bytes
#define ROUNDED_BLK_SIZE ((BLK_SIZE+7) & 0xFFFF'FFF8)
uint32_t regMulti[2][ROUNDED_BLK_SIZE] __attribute__((aligned(32)));

///////////////////////////////////////////////////////////////////
// Wrap the memcpy() so we can choose to use the CPU or DMA,
// which allows us to check DMA access to PSRAM, and also to
// check a buffer while copying another one 
///////////////////////////////////////////////////////////////////
DMAChannel copyDMA; // create and allocate DMA channel to use for memory copying
bool usingDMA;
void* dest;
size_t size;
uint32_t copyTimer;
float theCopyTime = 12345.67f;


// memcpy time in microseconds
void setTheCopyTime(uint32_t endTime) { theCopyTime = (float)(endTime - copyTimer) / F_CPU_ACTUAL * 1'000'000.0f; }

void* xmemcpy(void* dst, const void* src, size_t sz, bool useDMA = true)
{
  void * result = nullptr;

  if (!useDMA)
  {
    copyTimer = ARM_DWT_CYCCNT;
    result = memcpy(dst,src,sz);
    setTheCopyTime(ARM_DWT_CYCCNT);
    usingDMA = false;
  }
  else
  {
    // this assumes we're transferring 32-bit values!
    dest = dst; // store for cache clearance later
    size = sz;

    arm_dcache_flush((void*) src,size); // flush source out of cache to RAM
    arm_dcache_delete(dest,size); // delete cache record of destination contents
    copyDMA.destinationBuffer((uint32_t*) dst, sz);
    copyDMA.sourceBuffer((uint32_t*) src, sz);
    copyDMA.enable();
    copyDMA.disableOnCompletion();
    copyTimer = ARM_DWT_CYCCNT;
    copyDMA.triggerContinuously();
    usingDMA = true;
  }    

  return result;    
}

bool copyIsComplete(void) 
{ 
  bool result = true;

  if (usingDMA)
  {
    if (copyDMA.complete())
    {
      setTheCopyTime(ARM_DWT_CYCCNT);
    }
    else
      result = false;
  }

  return result;
}


bool new_fail_message(uint32_t* pm, volatile uint32_t *location, int count, int which)
{
  //Serial.printf(" Error at %08X, read %08X but expected %08X\n",
  //  (uint32_t)location, actual, expected);
  Serial.printf("Error at %08X\n",
                (uint32_t)location);
  int n = 16;
  uint32_t* pr = regMulti[which];
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
  for (int which = 0; which < 2; which++)
    for (int i = 0; i < BLK_SIZE; i++) regMulti[which][i] = pattern;
}


bool check_fixed_pattern(uint32_t pattern, bool useDMA)
{
  volatile uint32_t *p;
  int copyTimeState = 0;
  test_count++;
  Serial.printf("test %d with fixed pattern %08X%s", test_count, pattern, useDMA?", using DMA":"");


  p = memory_begin;
  nextRegFixed(pattern); // do once, value is fixed

  int which = 0;
  while (p < memory_end)
  {
    int count = memory_end - p; // words left to compare
    if (count > BLK_SIZE)
      count = BLK_SIZE;

    xmemcpy((void*) p, regMulti[which], count * sizeof *p, useDMA);
    while (!copyIsComplete()) {}
    if (0 == copyTimeState)
    {
      copyTimeState++;
      Serial.printf("; copy to memory at %.2fMB/s", BLK_SIZE*4 / theCopyTime);
    }

    p += count;
    which = 1-which;
  }

  arm_dcache_flush_delete((void *)memory_begin,
                          (uint32_t)memory_end - (uint32_t)memory_begin);

  p = memory_begin;
  which = 0;
  while (p < memory_end)
  {
    int cmpres = 999;
    uint32_t memBuff[BLK_SIZE];
    int count = memory_end - p; // words left to compare

    if (count > BLK_SIZE)
      count = BLK_SIZE;

    xmemcpy(memBuff, (void*) p, count * sizeof *p, useDMA);
    while (!copyIsComplete()) {}
    if (1 == copyTimeState)
    {
      copyTimeState++;
      Serial.printf("; from memory at %.2fMB/s\n", BLK_SIZE*4 / theCopyTime);
    }

    cmpres = memcmp(memBuff, regMulti[which], count * sizeof *p);
    p += count;

    if (0 != cmpres) return new_fail_message(memBuff, p - count, count, which);
    which = 1-which;
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


void nextRegMulti(int which)
{
  for (int i = 0; i < BLK_SIZE; i++)
    regMulti[which][i] = nextReg();
}


bool check_lfsr_pattern(uint32_t seed, bool useDMA)
{
  volatile uint32_t *p;
  int which = 0;
  int copyTimeState = 0;

  test_count++;
  Serial.printf("test %d with pseudo-random sequence, seed=%u%s", test_count, seed, useDMA?", using DMA":"");
  reg = seed;
  p = memory_begin;
  while (p < memory_end)
  {
    nextRegMulti(which);
    int count = memory_end - p;
    if (count > BLK_SIZE)
      count = BLK_SIZE;

    xmemcpy((void*) p, regMulti[which], count * sizeof *p, useDMA);
    while (!copyIsComplete()) {}
    if (0 == copyTimeState)
    {
      copyTimeState++;
      Serial.printf("; copy to memory at %.2fMB/s",BLK_SIZE*4 / theCopyTime);
    }

    p += count;
    which = 1-which;
  }

  arm_dcache_flush_delete((void *)memory_begin,
                          (uint32_t)memory_end - (uint32_t)memory_begin);

  reg = seed;
  p = memory_begin;
  which = 0;
  while (p < memory_end)
  {
    int cmpres = 999;
    uint32_t memBuff[BLK_SIZE];
    int count = memory_end - p;

    nextRegMulti(which);
    if (count >= BLK_SIZE)
      count = BLK_SIZE;
    const int sz = count * sizeof *p;

    xmemcpy(memBuff, (void*) p, sz, useDMA);
    while (!copyIsComplete()) {}
    if (1 == copyTimeState)
    {
      copyTimeState++;
      Serial.printf("; from memory at %.2fMB/s\n",BLK_SIZE*4 / theCopyTime);
    }

    cmpres = memcmp(memBuff, regMulti[which], sz);
    p += count;
    if (0 != cmpres) return new_fail_message(memBuff, p - count, count, which);
    which = 1-which;
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
