/* ***************************************************************** */
/* File name:        pid.h                                           */
/* File description: Header file containing the functions/methods    */
/*                   interfaces for handling the PID                 */
/* Author name:      julioalvesMS, IagoAF, rBacurau, Hebert Wandick  */
/*					 Caio Villela					                 */
/* Creation date:    21jun2018                                       */
/* Revision date:    01gos2020                                       */
/* ***************************************************************** */

#ifndef SOURCES_CONTROLLER_PID_H_
#define SOURCES_CONTROLLER_PID_H_

class PID
{
private:
    typedef struct pid_data_type
    {
        float fKp, fKi, fKd;   // PID gains
        float fError_previous; // used in the derivative
        float fError_sum;      // integrator cumulative error
        float fMin;
        float fMax;
    } pid_data_type;

    pid_data_type _pidConfig;
    float _fSetValue;
    int _iStaturationFlag;

public:
    PID(float fKp = 1, float fKi = 0, float fKd = 0, float fMin = 1, float fMax = -1);

    /* ************************************************** */
    /* Method name:        pid_setKp                      */
    /* Method description: Set a new value for the PID    */
    /*                     proportional constant          */
    /* Input params:       fKp: New value                 */
    /* Output params:      n/a                            */
    /* ************************************************** */
    void setKp(float fKp);

    /* ************************************************** */
    /* Method name:        pid_getKp                      */
    /* Method description: Get the value from the PID     */
    /*                     proportional constant          */
    /* Input params:       n/a                            */
    /* Output params:      float: Value                   */
    float getKp(void);

    /* ************************************************** */
    /* Method name:        pid_setKi                      */
    /* Method description: Set a new value for the PID    */
    /*                     integrative constant           */
    /* Input params:       fKi: New value                 */
    /* Output params:      n/a                            */
    /* ************************************************** */
    void setKi(float fKi);

    /* ************************************************** */
    /* Method name:        pid_getKi                      */
    /* Method description: Get the value from the PID     */
    /*                     integrative constant           */
    /* Input params:       n/a                            */
    /* Output params:      float: Value                   */
    /* ************************************************** */
    float getKi(void);

    /* ************************************************** */
    /* Method name:        pid_setKd                      */
    /* Method description: Set a new value for the PID    */
    /*                     derivative constant            */
    /* Input params:       fKd: New value                 */
    /* Output params:      n/a                            */
    /* ************************************************** */
    void setKd(float fKd);

    /* ************************************************** */
    /* Method name:        pid_getKd                      */
    /* Method description: Get the value from the PID     */
    /*                     derivative constant            */
    /* Input params:       n/a                            */
    /* Output params:      float: Value                   */
    /* ************************************************** */
    float getKd(void);

    /* ************************************************** */
    /* Method name:        pid_setSetValue                */
    /* Method description: set temperature target in PID  */
    /* Input params:       fValue: the temperature        */
    /* Output params:      n/a			                  */
    /* ************************************************** */
    void setSetValue(float fValue);

    /* ************************************************** */
    /* Method name:        pid_getSetValue                */
    /* Method description: get temperature target in PID  */
    /* Input params:       n/a					          */
    /* Output params:      float: temperature             */
    /* ************************************************** */
    float getSetValue();

    void setScaleMax(float fMax);

    void setScaleMin(float fMin);

    void reset();

    /* ************************************************** */
    /* Method name:        pid_updateData                 */
    /* Method description: Update the control output      */
    /*                     using the reference and sensor */
    /*                     value                          */
    /* Input params:       fSensorValue: Value read from  */
    /*                     the sensor                     */
    /*                     fReferenceValue: Value used as */
    /*                     control reference              */
    /* Output params:      float: New Control effort     */
    /* ************************************************** */
    float UpdateData(float fSensorValue);
};

#endif /* SOURCES_CONTROLLER_PID_H_ */