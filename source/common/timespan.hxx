#ifndef __SQHAO_TIIMESPAN_HXX__
#define __SQHAO_TIIMESPAN_HXX__

#include <utility>
#include "default_config.hxx"
NAMESPACE_PREFIX


class Timespan{
public:
	typedef long long int TimeDiff;

	Timespan():_span(0){}
		
	Timespan(TimeDiff microseconds):_span(microseconds){}
	
	Timespan(long seconds, long microseconds):
		_span(TimeDiff(seconds)*SECONDS + microseconds){}
		

	Timespan(int days, int hours, int minutes, int seconds, int microSeconds):
		_span(TimeDiff(microSeconds) + TimeDiff(seconds)*SECONDS + TimeDiff(minutes)*MINUTES + TimeDiff(hours)*HOURS + TimeDiff(days)*DAYS)
	{

	}

	Timespan(const Timespan& timespan):_span(timespan._span){}
	~Timespan()=default;
	Timespan& operator = (const Timespan& timespan){
		_span = timespan._span;
		return *this;
	}
	Timespan& operator = (TimeDiff microseconds){
		_span = microseconds;
		return *this;
	}
	Timespan& assign(int days, int hours, int minutes, int seconds, int microSeconds){
		_span = TimeDiff(microSeconds) + TimeDiff(seconds)*SECONDS + TimeDiff(minutes)*MINUTES + TimeDiff(hours)*HOURS + TimeDiff(days)*DAYS;
		return *this;
	}	
	Timespan& assign(long seconds, long microseconds){
		_span = TimeDiff(seconds)*SECONDS + TimeDiff(microseconds);
		return *this;
	}
	
	void swap(Timespan& timespan){
		std::swap(_span, timespan._span);
	}

	bool operator == (const Timespan& ts) const;
	bool operator != (const Timespan& ts) const;
	bool operator >  (const Timespan& ts) const;
	bool operator >= (const Timespan& ts) const;
	bool operator <  (const Timespan& ts) const;
	bool operator <= (const Timespan& ts) const;

	bool operator == (TimeDiff microSeconds) const;
	bool operator != (TimeDiff microSeconds) const;
	bool operator >  (TimeDiff microSeconds) const;
	bool operator >= (TimeDiff microSeconds) const;
	bool operator <  (TimeDiff microSeconds) const;
	bool operator <= (TimeDiff microSeconds) const;
	
	Timespan operator + (const Timespan& d) const;
	Timespan operator - (const Timespan& d) const;
	Timespan& operator += (const Timespan& d);
	Timespan& operator -= (const Timespan& d);

	Timespan operator + (TimeDiff microSeconds) const;
	Timespan operator - (TimeDiff microSeconds) const;
	Timespan& operator += (TimeDiff microSeconds);
	Timespan& operator -= (TimeDiff microSeconds);

	int days() const{
		return int(_span/DAYS);
	}
		
	int hours() const{
		return int((_span/HOURS) % 24);
	}
	int totalHours() const{
		return int(_span/HOURS);
	}
	int minutes() const{
		return int((_span/MINUTES) % 60);
	}
	int totalMinutes() const{
		return int(_span/MINUTES);
	}
	int seconds() const{
		return int((_span/SECONDS) % 60);
	}
	int totalSeconds() const{
		return int(_span/SECONDS);
	}
	int milliseconds() const{
		return int((_span/MILLISECONDS) % 1000);
	}
	TimeDiff totalMilliseconds() const{
		return _span/MILLISECONDS;
	}
	/// microseconds 0~999
	int microseconds() const{
		return int(_span % 1000);
	}
	int useconds() const{
		return int(_span % 1000000);
	}
	TimeDiff totalMicroseconds() const{
		return _span;
	}

	static const TimeDiff MILLISECONDS; /// The number of microseconds in a millisecond.
	static const TimeDiff SECONDS;      /// The number of microseconds in a second.
	static const TimeDiff MINUTES;      /// The number of microseconds in a minute.
	static const TimeDiff HOURS;        /// The number of microseconds in a hour.
	static const TimeDiff DAYS;         /// The number of microseconds in a day.

private:
	TimeDiff _span;   // microseconds 
};


inline bool Timespan::operator == (const Timespan& ts) const
{
	return _span == ts._span;
}


inline bool Timespan::operator != (const Timespan& ts) const
{
	return _span != ts._span;
}


inline bool Timespan::operator >  (const Timespan& ts) const
{
	return _span > ts._span;
}


inline bool Timespan::operator >= (const Timespan& ts) const
{
	return _span >= ts._span;
}


inline bool Timespan::operator <  (const Timespan& ts) const
{
	return _span < ts._span;
}


inline bool Timespan::operator <= (const Timespan& ts) const
{
	return _span <= ts._span;
}


inline bool Timespan::operator == (TimeDiff microSeconds) const
{
	return _span == microSeconds;
}


inline bool Timespan::operator != (TimeDiff microSeconds) const
{
	return _span != microSeconds;
}


inline bool Timespan::operator >  (TimeDiff microSeconds) const
{
	return _span > microSeconds;
}


inline bool Timespan::operator >= (TimeDiff microSeconds) const
{
	return _span >= microSeconds;
}


inline bool Timespan::operator <  (TimeDiff microSeconds) const
{
	return _span < microSeconds;
}


inline bool Timespan::operator <= (TimeDiff microSeconds) const
{
	return _span <= microSeconds;
}


inline void swap(Timespan& s1, Timespan& s2)
{
	s1.swap(s2);
}


NAMESPACE_SUBFIX


#endif // __SQHAO_TIIMESPAN_HXX__
