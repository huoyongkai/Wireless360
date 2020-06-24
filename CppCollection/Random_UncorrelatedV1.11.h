
/**
 * @file
 * @brief Independent random value generation
 * @version 1.11
 * @author Yongkai HUO, yh3g09 (forevervhuo@gmail.com, yh3g09@ecs.soton.ac.uk)
 * @date Feb 21, 2010-Oct 26, 2011
 * @copyright None.
*/

#ifndef RANDOM_UNCORRELATED_H_
#define RANDOM_UNCORRELATED_H_
#include <itpp/itcomm.h>
#include <itpp/itbase.h>
//#include <itpp/base/operators.h>
#include <ctime>
using namespace itpp;
using namespace std;

/**
 * @brief Uncorrelated random class
 * @details you can use many objects independently. Modified from IT++ libarary
*/
class Random_Uncorrelated
{
public:
    //! Construct a new Random_Generator.
    Random_Uncorrelated() {
        reset(4357U);
    }
    //! Construct Random_Generator object using \c seed
    Random_Uncorrelated(unsigned int seed) {
        reset(seed);
    }
    //! Set the seed to a semi-random value (based on hashed time and clock).
    void randomize() {
        time_t temp;
        time(&temp);
        reset(temp);
    }
    //! Reset the source. The same sequance will be generated as the last time.
    void reset() {
        initialize(last_seed);
        reload();
    }
    //! Reset the source after setting the seed to seed.
    void reset(unsigned int seed) {
        last_seed = seed;
        reset();
    }

    //! Return a uniformly distributed [0,2^32-1] integer.
    unsigned int random_int() {
        if (left == 0) reload();
        --left;

        register unsigned int s1;
        s1 = *pNext++;
        s1 ^= (s1 >> 11);
        s1 ^= (s1 <<  7) & 0x9d2c5680U;
        s1 ^= (s1 << 15) & 0xefc60000U;
        return (s1 ^(s1 >> 18));
    }

    //! Return a uniformly distributed (0,1) value.
    double random_01() {
        return (random_int() + 0.5) * (1.0 / 4294967296.0);
    }
    //! Return a uniformly distributed (0,1] value.
    double random_01_rclosed() {
        return (random_int() + 0.1) * (1.0 / 4294967295.1);    //added by Yongkai Huo on Feb.22 2010
    }
    //! Return a uniformly distributed [0,1) value.
    double random_01_lclosed() {
        return random_int() * (1.0 / 4294967296.0);
    }
    //! Return a uniformly distributed [0,1] value.
    double random_01_closed() {
        return random_int() * (1.0 / 4294967295.0);
    }
    //! Return a uniformly distributed [0,1) value in 53-bit resolution.
    double random53_01_lclosed() {
        return ((random_int() >> 5) * 67108864.0 + (random_int() >> 6))
               * (1.0 / 9007199254740992.0); // by Isaku Wada
    }

    //! Save current full state of generator in memory
    void get_state(ivec &out_state);
    //! Resume the state saved in memory. Clears memory.
    void set_state(ivec &new_state);

private:
    //! seed used for initialisation
    int last_seed;
    //! internal state
    unsigned int state[624];
    //! next value to get from state
    unsigned int *pNext;
    //! number of values left before reload needed
    int left;

    /*!
     * \brief Initialize generator state with seed.
     * See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
     * \note In previous versions, most significant bits (MSBs) of the seed
     * affect only MSBs of the state array. Modified 9 Jan 2002 by Makoto
     * Matsumoto.
     */
    void initialize(unsigned int seed) {
        register unsigned int *s = state;
        register unsigned int *r = state;
        register int i = 1;
        *s++ = seed & 0xffffffffU;
        for (; i < 624; ++i) {
            *s++ = (1812433253U * (*r ^(*r >> 30)) + i) & 0xffffffffU;
            r++;
        }
    }

    /*!
     * \brief Generate N new values in state.
     * Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
     */
    void reload() {
        register unsigned int *p = state;
        register int i;
        for (i = 624 - 397; i--; ++p)
            *p = twist(p[397], p[0], p[1]);
        for (i = 397; --i; ++p)
            *p = twist(p[397-624], p[0], p[1]);
        *p = twist(p[397-624], p[0], state[0]);

        left = 624, pNext = state;
    }
    //!
    unsigned int hiBit(const unsigned int& u) const {
        return u & 0x80000000U;
    }
    //!
    unsigned int loBit(const unsigned int& u) const {
        return u & 0x00000001U;
    }
    //!
    unsigned int loBits(const unsigned int& u) const {
        return u & 0x7fffffffU;
    }
    //!
    unsigned int mixBits(const unsigned int& u, const unsigned int& v) const
    {
        return hiBit(u) | loBits(v);
    }

    /*
     * ----------------------------------------------------------------------
     * --- ediap - 2007/01/17 ---
     * ----------------------------------------------------------------------
     * Wagners's implementation of the twist() function was as follows:
     *  { return m ^ (mixBits(s0,s1)>>1) ^ (-loBit(s1) & 0x9908b0dfU); }
     * However, this code caused a warning/error under MSVC++, because
     * unsigned value loBit(s1) is being negated with `-' (c.f. bug report
     * [1635988]). I changed this to the same implementation as is used in
     * original C sources of Mersenne Twister RNG:
     *  #define MATRIX_A 0x9908b0dfUL
     *  #define UMASK 0x80000000UL
     *  #define LMASK 0x7fffffffUL
     *  #define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
     *  #define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))
     * ----------------------------------------------------------------------
     */
    //!
    unsigned int twist(const unsigned int& m, const unsigned int& s0,
                       const unsigned int& s1) const
    {
        return m ^(mixBits(s0, s1) >> 1) ^(loBit(s1) ? 0x9908b0dfU : 0U);
    }
    //!
// unsigned int hash(time_t t, clock_t c);
};

#endif /* RANDOM_UNCORRELATED_H_ */
