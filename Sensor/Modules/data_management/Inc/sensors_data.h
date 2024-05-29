#ifndef SENSORS_DATA_H 
#define SENSORS_DATA_H

#define UINT16_MAX_VALUE (2 << 16)

struct fixed_point_float {
    uint16_t integer_part; 
    uint16_t mantissa_part;
};

struct sensors_data {
    struct fixed_point_float pressure;
    struct fixed_point_float temp;
    struct fixed_point_float humidity;
#if 0
    float_value accelerometer_x;
    float_value accelerometer_y;
    float_value accelerometer_z;
    float_value gyroscope_x;
    float_value gyroscope_y;
    float_value gyroscope_z;
#endif
};

static inline void float_to_fixed_point(struct fixed_point_float *fp, float value) {
    fp->integer_part = (uint16_t)value;
    fp->mantissa_part = (uint16_t)((float)(value - fp->integer_part) * UINT16_MAX_VALUE);
}

#endif // SENSORS_DATA_H