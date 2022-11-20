#pragma once
#include "AudioTools/AudioTypes.h"
#include "AudioBasic/Collections.h"
#include "AudioFilter/Filter.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct fir_f32_s {
    float  *coeffs;     /*!< Pointer to the coefficient buffer.*/
    float  *delay;      /*!< Pointer to the delay line buffer.*/
    int     N;          /*!< FIR filter coefficients amount.*/
    int     pos;        /*!< Position in delay line.*/
    int     decim;      /*!< Decimation factor.*/
    int     d_pos;      /*!< Actual decimation counter.*/
} fir_f32_t;

int dsps_fir_f32_ae32(fir_f32_t* fir, const float* input, float* output, int len);

#ifdef __cplusplus
}
#endif


namespace audio_tools {


/**
 * @brief Converter for n Channels which applies the indicated Filter
 * @author pschatzmann
 * @tparam T
 */

template <typename T>
class FIRConverter : public BaseConverter<T> {
 public:

  FIRConverter( float* coeffsLeft, float* coeffsRight, int firLen ) {

    this->channels = 2;
    this->direction = 1;
    this->gain = 1;
    this->correction = 1;

    float* delayLeft = (float*)calloc( firLen, sizeof(float) );
    float* delayRight = (float*)calloc( firLen, sizeof(float) );

    fir_init( &firLeft, coeffsLeft, delayLeft, firLen );
    fir_init( &firRight, coeffsRight, delayRight, firLen );

    int n = DEFAULT_BUFFER_SIZE;

    srcLeft = (float*)calloc( n, sizeof(float) );
    srcRight = (float*)calloc( n, sizeof(float) );
	destLeft = (float*)calloc( n, sizeof(float) );
	destRight = (float*)calloc( n, sizeof(float) );

  }

  void setDirection( int direction ) {
	  this->direction = direction;
  }

  void setGain( int gain ) {
	  this->gain = gain;
  }

  void setCorrection( float correction ) {
	  this->correction = correction;
  }

  ~FIRConverter() {
  }

  void fir_init( fir_f32_t *fir, float *coeffs, float *delay, int N )
  {
      fir->coeffs = coeffs;
      fir->delay = delay;
      fir->N = N;
      fir->pos = 0;

      for (int i = 0 ; i < N; i++) {
          fir->delay[i] = 0;
      }
  }

  void fir(fir_f32_t *fir, float *input, float *output, int len)
  {
      for (int i = 0 ; i < len ; i++) {
          float acc = 0;
          int coeff_pos = fir->N - 1;
          fir->delay[fir->pos] = input[i];
          fir->pos++;
          if (fir->pos >= fir->N) {
              fir->pos = 0;
          }
          for (int n = fir->pos; n < fir->N ; n++) {
              acc += fir->coeffs[coeff_pos--] * fir->delay[n];
          }
          for (int n = 0; n < fir->pos ; n++) {
              acc += fir->coeffs[coeff_pos--] * fir->delay[n];
          }
          output[i] = acc;
      }
  }

  size_t convert(uint8_t *src, size_t size) {

    int count = size / channels / sizeof(T);
    T *sample = (T *)src;

    for ( int i = 0 ; i < count ; i++ )
    {
    	srcLeft[i] = sample[i*2] * correction;
    	srcRight[i] = sample[i*2+1];
    }

    dsps_fir_f32_ae32(&firLeft, srcLeft, destLeft, count);
    dsps_fir_f32_ae32(&firRight, srcRight, destRight, count);

    write( sample, count );

    return size;
  }

  virtual void write( T* sample, int count )
  {
    for ( int i = 0 ; i < count ; i++ )
    {
    	sample[i*2] = gain * destLeft[i];
    	sample[i*2+1] = gain * destRight[i];
    }
  }


 protected:
  int direction;
  int gain;
  int channels;
  float correction;

  float* 	coeffsLeft;
  float* 	coeffsRight;
  int		firLen;

  float*	delayLeft;
  float*	delayRight;

  float*	srcLeft;
  float*	srcRight;
  float*	destLeft;
  float*	destRight;

  fir_f32_t firLeft;
  fir_f32_t firRight;

  int loop = 0;
};

template <typename T>
class FIRAddConverter : public FIRConverter<T> {

public:
	  FIRAddConverter( float* coeffsLeft, float* coeffsRight, int firLen ) :
	  	  FIRConverter<T>( coeffsLeft, coeffsRight, firLen )
	  {}

	  virtual void write( T* sample, int count )
	  {
	    for ( int i = 0 ; i < count ; i++ )
	    {
	    	T out = this->gain * ( this->destLeft[i] + this->direction * this->destRight[i] );

	    	sample[i*2] = out;
	    	sample[i*2+1] = out;
	    }
	  }

};


}
