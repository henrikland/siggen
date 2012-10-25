#include <wave.h>

void oscil_init(OSCIL* osc, unsigned long srate)
{
	osc->twopiovrsr = TWOPI / (double) srate;
	osc->curfreq = 0.0;
	osc->curphase = 0.0;
	osc->incr = 0.0;
}

double sinetick(OSCIL* osc, double freq)
{
	double val;	
	if (osc->curfreq != freq) {
		osc->curfreq = freq;
		osc->incr = freq * osc->twopiovrsr;
	}

	val = sin(osc->curphase);
	osc->curphase += osc->incr;
	if (osc->curphase >= TWOPI) osc->curphase -= TWOPI;
	if (osc->curphase < 0.0) osc->curphase += TWOPI;

	return val;
}

double squaretick(OSCIL* osc, double freq)
{
	double val;
	if (osc->curfreq != freq) {
		osc->curfreq = freq;
		osc->incr = freq * osc->twopiovrsr;
	}

	val = (osc->curphase <= M_PI) ? 1.0 : -1.0;
	osc->curphase += osc->incr;
	if (osc->curphase >= TWOPI) osc->curphase -= TWOPI;
	if (osc->curphase < 0.0) osc->curphase += TWOPI;

	return val;
}

double sawdowntick(OSCIL* osc, double freq)
{
	double val;
	if (osc->curfreq != freq) {
		osc->curfreq = freq;
		osc->incr = freq * osc->twopiovrsr;
	}

	val = 1.0 - 2.0 * (osc->curphase / TWOPI);
	osc->curphase += osc->incr;
	if (osc->curphase >= TWOPI) osc->curphase -= TWOPI;
	if (osc->curphase < 0.0) osc->curphase += TWOPI;

	return val;
}

double sawuptick(OSCIL* osc, double freq)
{
	double val;
	if (osc->curfreq != freq) {
		osc->curfreq = freq;
		osc->incr = freq * osc->twopiovrsr;
	}

	val = 2.0 * (osc->curphase / TWOPI) - 1.0;
	osc->curphase += osc->incr;
	if (osc->curphase >= TWOPI) osc->curphase -= TWOPI;
	if (osc->curphase < 0.0) osc->curphase += TWOPI;

	return val;
}

double pwmtick(OSCIL* osc, double freq, double pwm)
{
	double val;
	if (osc->curfreq != freq) {
		osc->curfreq = freq;
		osc->incr = freq * osc->twopiovrsr;
	}
	val = (osc->curphase <= (TWOPI * pwm)) ? 1.0 : -1.0;
	osc->curphase += osc->incr;
	if (osc->curphase >= TWOPI) osc->curphase -= TWOPI;
	if (osc->curphase < 0.0) osc->curphase += TWOPI;

	return val;
}