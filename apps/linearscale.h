#pragma once

#include <iostream>
#include <stdexcept>

/*! @brief Changes input range into output one using a linear scale.
 *
 * Example, we desire to change the real range from [-PI,PI] to [0,256]
 *
 * LinearScale scaler(-PI,PI,0,256);
 */
class LinearScale
{
public:
	/*!@brief Parametrized constructor. Creates a scaler from [_inMin,_inMax] to  [_outMin,_outMax].
	 * @param[in] inMin minimum value of input range
	 * @param[in] inMax maximum value of input range
	 * @param[in] outMin minimum value of output range
	 * @param[in] outMax   maximum value of output  range
	 */
	LinearScale(const double inMin, const double inMax, const double outMin, const double outMax);

	// ====================  OPERATORS     =========================
	/*!@brief Perform the transform.
	 * @param in value in the range [_inMin,_inMax]
	 * @return change of scale into [_outMin,_outMax]
	 */
	double operator()(double in) const;

private:
	double _minIn;
	double _maxIn;
	double _minOut;
	double _maxOut;
	double _a, _b;        ///< Function parameters y=a*x+b
};
