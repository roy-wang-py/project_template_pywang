
#include "timespan.hxx"
#include <algorithm>


NAMESPACE_PREFIX


const Timespan::TimeDiff Timespan::MILLISECONDS = 1000;
const Timespan::TimeDiff Timespan::SECONDS      = 1000*Timespan::MILLISECONDS;
const Timespan::TimeDiff Timespan::MINUTES      =   60*Timespan::SECONDS;
const Timespan::TimeDiff Timespan::HOURS        =   60*Timespan::MINUTES;
const Timespan::TimeDiff Timespan::DAYS         =   24*Timespan::HOURS;



Timespan Timespan::operator + (const Timespan& d) const
{
	return Timespan(_span + d._span);
}


Timespan Timespan::operator - (const Timespan& d) const
{
	return Timespan(_span - d._span);
}


Timespan& Timespan::operator += (const Timespan& d)
{
	_span += d._span;
	return *this;
}


Timespan& Timespan::operator -= (const Timespan& d)
{
	_span -= d._span;
	return *this;
}


Timespan Timespan::operator + (TimeDiff microSeconds) const
{
	return Timespan(_span + microSeconds);
}


Timespan Timespan::operator - (TimeDiff microSeconds) const
{
	return Timespan(_span - microSeconds);
}


Timespan& Timespan::operator += (TimeDiff microSeconds)
{
	_span += microSeconds;
	return *this;
}


Timespan& Timespan::operator -= (TimeDiff microSeconds)
{
	_span -= microSeconds;
	return *this;
}


NAMESPACE_SUBFIX
