#include "linearscale.h"

LinearScale::LinearScale(const double inMin, const double inMax,
						 const double outMin, const double outMax) :
	_minIn ( inMin),
	_maxIn ( inMax),
	_minOut ( outMin),
	_maxOut ( outMax)
{
	double aux = (inMax - inMin);
	if(aux != 0.0) {
		_a = (outMax - outMin) / aux;
		_b = outMax - (_a * inMax);
	} else
		_a = _b = 0.0;
}

double LinearScale::operator()(double in) const
{
	if(in < _minIn)
		return _minOut;
	else if(in > _maxIn)
		return _maxOut;

	return in * _a + _b;
}
