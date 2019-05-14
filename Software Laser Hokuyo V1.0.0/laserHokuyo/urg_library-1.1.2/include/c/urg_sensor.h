#ifndef URG_SENSOR_H
#define URG_SENSOR_H

/*!
  \file
  \author Satofumi KAMIMURA

  $Id: urg_sensor.h,v 540bc11f70c8 2011/05/08 23:04:49 satofumi $
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "urg_connection.h"

    /*!
    */
    typedef enum {
        URG_DISTANCE,           /*!< */
        URG_DISTANCE_INTENSITY, /*!< */
        URG_MULTIECHO,          /*!< */
        URG_MULTIECHO_INTENSITY, /*!< */
        URG_STOP,                /*!< */
        URG_UNKNOWN,             /*!< */
    } urg_measurement_type_t;

    /*!
    */
    typedef enum {
        URG_COMMUNICATION_3_BYTE, /*!< */
        URG_COMMUNICATION_2_BYTE, /*!< */
    } urg_range_data_byte_t;


    enum {
        URG_SCAN_INFINITY = 0,  /*!< */
        URG_MAX_ECHO = 3, /*!< */
    };


    /*! */
    typedef urg_measurement_type_t
    (*urg_error_handler)(const char *status, void *urg);


    /*!
    */
    typedef struct
    {
        int is_active;
        int last_errno;
        urg_connection_t connection;

        int first_data_index;
        int last_data_index;
        int front_data_index;
        int area_resolution;
        long scan_usec;
        int min_distance;
        int max_distance;
        int scanning_first_step;
        int scanning_last_step;
        int scanning_skip_step;
        int scanning_skip_scan;
        urg_range_data_byte_t range_data_byte;

        int timeout;
        int specified_scan_times;
        int scanning_remain_times;
        int is_laser_on;

        int received_first_index;
        int received_last_index;
        int received_skip_step;
        urg_range_data_byte_t received_range_data_byte;
        int is_sending;

        urg_error_handler error_handler;

        char return_buffer[80];
    } urg_t;


    /*!
      \see urg_close()
    */
    extern int urg_open(urg_t *urg, urg_connection_type_t connection_type,
                        const char *device_or_address,
                        long baudrate_or_port);


    /*!
      \see urg_open()
    */
    extern void urg_close(urg_t *urg);


    /*!
      \brief タイムアウト時間の設定

      \param[in,out] urg URG センサ管理
      \param[in] msec タイムアウトする時間 [msec]

      \attention urg_open() を呼び出すと timeout の設定値はデフォルト値に初期化されるため、この関数は urg_open() 後に呼び出すこと。
    */
    extern void urg_set_timeout_msec(urg_t *urg, int msec);


    /*! */
    extern int urg_start_time_stamp_mode(urg_t *urg);


    /*!
    */
    extern long urg_time_stamp(urg_t *urg);


    /*! */
    extern int urg_stop_time_stamp_mode(urg_t *urg);


    /*!
      \see urg_get_distance(), urg_get_distance_intensity(), urg_get_multiecho(), urg_get_multiecho_intensity(), urg_stop_measurement()
    */
    extern int urg_start_measurement(urg_t *urg, urg_measurement_type_t type,
                                     int scan_times, int skip_scan);


    /*!
      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_distance(urg_t *urg, long data[], long *time_stamp);


    /*!
      Example
      \code
      int data_size = urg_max_data_size(&urg);
      long *data = malloc(data_size * sizeof(long));
      long *intensity = malloc(data_size * sizeof(unsigned short));

      ...

      urg_start_measurement(&urg, URG_DISTANCE_INTENSITY, 1, 0);
      int n = urg_get_distance_intensity(&urg, data, intesnity, NULLL); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_distance_intensity(urg_t *urg, long data[],
                                          unsigned short intensity[],
                                          long *time_stamp);


    /*!
      Example
      \code
      long *data_multi = malloc(3 * urg_max_data_size(&urg) * sizeof(long));

      ...

      urg_start_measurement(&urg, URG_MULTIECHO, 1, 0);
      int n = urg_get_distance_intensity(&urg, data_multi, NULLL); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_multiecho(urg_t *urg, long data_multi[], long *time_stamp);


    /*!
      Example
      \code
      int data_size = urg_max_data_size(&urg);
      long *data_multi = malloc(3 * data_size * sizeof(long));
      long *intensity_multi = malloc(3 * data_size * sizeof(unsigned short));

      ...

      urg_start_measurement(&urg, URG_DISTANCE_INTENSITY, 1, 0);
      int n = urg_get_multiecho_intensity(&urg, data_multi,
      intesnity_multi, NULLL); \endcode

      \see urg_start_measurement(), urg_max_data_size()
    */
    extern int urg_get_multiecho_intensity(urg_t *urg, long data_multi[],
                                           unsigned short intensity_multi[],
                                           long *time_stamp);


    /*!
      Example
      \code
      urg_start_measurement(&urg, URG_DISTANCE, URG_SCAN_INFINITY, 0);
      for (int i = 0; i < 10; ++i) {
      urg_get_distance(&urg, data, NULL);
      }
      urg_stop_measurement(&urg); \endcode

      \see urg_start_measurement()
    */
    extern int urg_stop_measurement(urg_t *urg);


    /*!
      Example
      \code
      urg_set_scanning_parameter(&urg, urg_deg2step(&urg, -45),
      urg_deg2step(&urg, +45), 1);
      urg_start_measurement(&urg, URG_DISTANCE, 0);
      int n = urg_get_distance(&urg, data, NULL);
      for (int i = 0; i < n; ++i) {
      printf("%d [mm], %d [deg]\n", data[i], urg_index2deg(&urg, i));
      } \endcode

      \see urg_step_min_max(), urg_rad2step(), urg_deg2step()
    */
    extern int urg_set_scanning_parameter(urg_t *urg, int first_step,
                                          int last_step, int skip_step);


    /*!
    */
    extern int urg_set_communication_data_size(urg_t *urg,
                                               urg_range_data_byte_t data_byte);


    /*! */
    extern int urg_laser_on(urg_t *urg);


    /*! */
    extern int urg_laser_off(urg_t *urg);


    /*! */
    extern int urg_reboot(urg_t *urg);


    /*!
    */
    extern void urg_sleep(urg_t *urg);


    /*!
    */
    extern void urg_wakeup(urg_t *urg);

    /*!
    */
    extern int urg_is_stable(urg_t *urg);


    /*!
    */
    extern const char *urg_sensor_product_type(urg_t *urg);


    /*!
    */
    extern const char *urg_sensor_serial_id(urg_t *urg);


    /*!
    */
    extern const char *urg_sensor_firmware_version(urg_t *urg);


    /*!
    */
    extern const char *urg_sensor_status(urg_t *urg);


    /*!
    */
    extern const char *urg_sensor_state(urg_t *urg);


    /*!
    */
    extern void urg_set_error_handler(urg_t *urg, urg_error_handler handler);


    /*!
    */
    extern long urg_scip_decode(const char data[], int size);


#ifdef __cplusplus
}
#endif

#endif /* !URG_SENSOR_H */
