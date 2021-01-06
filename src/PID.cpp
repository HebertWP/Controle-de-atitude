/* ***************************************************************** */
/* File name:        pid.c                                           */
/* File description: This file has a couple of useful functions to   */
/*                   control the implemented PID controller          */
/* Author name:      julioalvesMS, IagoAF, rBacurau, Hebert Wandick  */
/*					 Caio Villela					                 */
/* Creation date:    21jun2018                                       */
/* Revision date:    01ago2020                                       */
/* ***************************************************************** */

#include "PID.h"

PID::PID(float fKp, float fKi, float fKd, float fMin, float fMax)
{
    this->setKp(fKp);
    this->setKi(fKi);
    this->setKd(fKd);
    this->setSetValue(0);
    this->setScaleMax(fMax);
    this->setScaleMin(fMin);
    this->reset();
    _SataturationFlagUP = false;
    _SataturationFlagDOWN = false;
}

/* ************************************************** */
/* Method name:        pid_setKp                      */
/* Method description: Set a new value for the PID    */
/*                     proportional constant          */
/* Input params:       fKp: New value                 */
/* Output params:      n/a                            */
/* ************************************************** */
void PID::setKp(float fKp)
{
    _pidConfig.fKp = fKp;
}

/* ************************************************** */
/* Method name:        pid_getKp                      */
/* Method description: Get the value from the PID     */
/*                     proportional constant          */
/* Input params:       n/a                            */
/* Output params:      float: Value                   */
/* ************************************************** */
float PID::getKp(void)
{
    return _pidConfig.fKp;
}

/* ************************************************** */
/* Method name:        pid_setKi                      */
/* Method description: Set a new value for the PID    */
/*                     integrative constant           */
/* Input params:       fKi: New value                 */
/* Output params:      n/a                            */
/* ************************************************** */
void PID::setKi(float fKi)
{
    _pidConfig.fKi = fKi;
}

/* ************************************************** */
/* Method name:        pid_getKi                      */
/* Method description: Get the value from the PID     */
/*                     integrative constant           */
/* Input params:       n/a                            */
/* Output params:      float: Value                   */
/* ************************************************** */
float PID::getKi(void)
{
    return _pidConfig.fKi;
}

/* ************************************************** */
/* Method name:        pid_setKd                      */
/* Method description: Set a new value for the PID    */
/*                     derivative constant            */
/* Input params:       fKd: New value                 */
/* Output params:      n/a                            */
/* ************************************************** */
void PID::setKd(float fKd)
{
    _pidConfig.fKd = fKd;
}

/* ************************************************** */
/* Method name:        pid_getKd                      */
/* Method description: Get the value from the PID     */
/*                     derivative constant            */
/* Input params:       n/a                            */
/* Output params:      float: Value                   */
/* ************************************************** */
float PID::getKd(void)
{
    return _pidConfig.fKd;
}

/* ************************************************** */
/* Method name:        pid_setSetValue                */
/* Method description: set temperature target in PID  */
/* Input params:       fValue: the temperature        */
/* Output params:      n/a			                  */
/* ************************************************** */
void PID::setSetValue(float fValue)
{
    _fSetValue = fValue; /*limit temperature in 77*/
}

/* ************************************************** */
/* Method name:        pid_getSetValue                */
/* Method description: get temperature target in PID  */
/* Input params:       n/a					          */
/* Output params:      float: temperature             */
/* ************************************************** */
float PID::getSetValue()
{
    return _fSetValue;
};

void PID::setScaleMax(float fMax)
{
    _pidConfig.fMax = fMax;
}

void PID::setScaleMin(float fMin)
{
    _pidConfig.fMin = fMin;
}
float PID::getScaleMin()
{
    return _pidConfig.fMin;
}

float PID::getScaleMax()
{
    return _pidConfig.fMax;
}

void PID::reset()
{
    _SataturationFlagUP = false;
    _SataturationFlagDOWN = false;
    _pidConfig.fError_previous = 0;
    _pidConfig.fError_sum = 0;
}

/* ************************************************** */
/* Method name:        pid_updateData                 */
/* Method description: Update the control output      */
/*                     using the reference and sensor */
/*                     value                          */
/* Input params:       fSensorValue: Value read from  */
/*                     the sensor                     */
/* Output params:      float: New Control effort     */
/* ************************************************** */
float PID::UpdateData(float fSensorValue)
{
    float fError, fDifference, fOut;

    fError = _fSetValue - fSensorValue;

    /* if there is no saturation */
    if ((!_SataturationFlagUP && fError > 0) ||
        (!_SataturationFlagDOWN && fError < 0))
        _pidConfig.fError_sum += fError;
    
    fDifference = _pidConfig.fError_previous - fError;

    fOut = _pidConfig.fKp * fError + _pidConfig.fKi * _pidConfig.fError_sum + _pidConfig.fKd * fDifference;

    _pidConfig.fError_previous = fError;

    if (fOut > _pidConfig.fMax)
    {
        fOut = _pidConfig.fMax;
        _SataturationFlagUP = true;
    }
    else
    {
        _SataturationFlagUP = false;
    }
    
    if (fOut < _pidConfig.fMin)
    {
        fOut = _pidConfig.fMin;
        _SataturationFlagDOWN = true;
    }
    else
        _SataturationFlagDOWN = false;
    return fOut;
}