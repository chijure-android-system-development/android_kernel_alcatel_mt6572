

#if defined(JRD_DIGICEL_AUDIO_PARAM)
#include "Digicel/sph_coeff_dmnr_default.h"

#elif defined(JRD_PANASONIC_AUDIO_PARAM)
#include "Panasonic/sph_coeff_dmnr_default.h"

#elif !defined(MTK_2IN1_SPK_SUPPORT)
#include "EU/sph_coeff_dmnr_default.h"

#else
#include "US/sph_coeff_dmnr_default.h"
#endif
