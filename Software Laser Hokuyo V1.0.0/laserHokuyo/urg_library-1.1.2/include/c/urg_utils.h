#ifndef URG_UTILS_H
#define URG_UTILS_H

/*!
  \file
  \author Satofumi KAMIMURA

  $Id: urg_utils.h,v 630ee326c5ce 2011/02/19 08:06:25 Satofumi $
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "urg_sensor.h"


    /*!
      Example
      \code
      if (!urg_open(&urg, "/dev/ttyACM0", 115200, URG_SERIAL)) {
      printf("urg_open: %s\n", urg_error(&urg));
      return -1;
      } \endcode
    */
    extern const char *urg_error(const urg_t *urg);


    /*!
      Example
      \code
      long min_distance, max_distance;
      urg_distance_min_max(&urg, &min_distance, &max_distance);

      for (int i = 0; i < n; ++i) {
      long distance = data[i];
      if ((distance < min_distance) || (distance > max_distance)) {
      continue;
      }
      ...
      } \endcode
    */
    extern void urg_distance_min_max(const urg_t *urg,
                                     long *min_distance, long *max_distance);


    /*!
      Example
      \code
      urg_step_min_max(&urg, &min_step, &max_step);

      printf("range first: %d [deg]\n", urg_step2deg(&urg, min_step));
      printf("range last : %d [deg]\n", urg_step2deg(&urg, max_step)); \endcode

      \see urg_set_scanning_parameter(), urg_step2rad(), urg_step2deg()
    */
    extern void urg_step_min_max(const urg_t *urg, int *min_step, int *max_step);


    /*! */
    extern long urg_scan_usec(const urg_t *urg);


    /*! */
    extern int urg_max_data_size(const urg_t *urg);


    /*!
      Example
      \code
      int n = urg_get_distance(&urg, data, NULL);
      for (int i = 0; i < n; ++i) {
      long distance = data[i];
      double radian = urg_index2rad(i);
      double x = distance * cos(radian);
      double y = distance * sin(radian);
      printf("%.1f, %.1f\n", x, y);
      } \endcode

      \see urg_index2deg(), urg_rad2index(), urg_deg2index()
    */
    extern double urg_index2rad(const urg_t *urg, int index);


    /*! */
    extern double urg_index2deg(const urg_t *urg, int index);


    /*! */
    extern int urg_rad2index(const urg_t *urg, double radian);


    /*! */
    extern int urg_deg2index(const urg_t *urg, double degree);


    /*!
      \see urg_step_min_max(), urg_deg2step(), urg_step2rad(), urg_step2deg()
    */
    extern int urg_rad2step(const urg_t *urg, double radian);


    /*! */
    extern int urg_deg2step(const urg_t *urg, double degree);


    /*! */
    extern double urg_step2rad(const urg_t *urg, int step);


    /*! */
    extern double urg_step2deg(const urg_t *urg, int step);

    /*! */
    extern int urg_step2index(const urg_t *urg, int step);

#ifdef __cplusplus
}
#endif

#endif /* !URG_UTILS_H */
