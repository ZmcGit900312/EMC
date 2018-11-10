/*
	Copyright (C) 2004-2005  Timothy C.A. Molteno
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __nec_results__
#define __nec_results__

#include <vector>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <string>

#include "math_util.h"
using namespace std;

enum RESULT_FORMAT
{
	RESULT_FORMAT_NEC = 1,
	RESULT_FORMAT_XML = 2,
	RESULT_FORMAT_CSV = 3
};


/*!\brief A class that handles various standard output functions for the results.

This class will handle format changes between various output formats.
*/
class output_helper
{
private:
	ostream& os;
	enum RESULT_FORMAT m_format;
	
public:

	output_helper(ostream& in_os, enum RESULT_FORMAT in_format)
		: os(in_os), m_format(in_format)
	{
	}

	inline void separator()
	{
		switch (m_format)
		{
			case RESULT_FORMAT_CSV:
				os << ",";
				break;
			
			case RESULT_FORMAT_NEC:
			default:
				os << " ";
				break;
		}
	}
	
	inline void start_record()
	{
		switch (m_format)
		{
			case RESULT_FORMAT_XML:
				os << "<record>";
				break;
			
			default:
				break;
		}
	}
	inline void end_record()
	{
		switch (m_format)
		{
			case RESULT_FORMAT_XML:
				os << "<record>" << endl;
				break;
			
			default:
				os << endl;
				break;
		}
	}
	
	inline void padding(char* s)
	{
		switch (m_format)
		{
			case RESULT_FORMAT_NEC:
				os << s;
				break;
			
			default:
				break;
		}
	}
	
	inline void section_start()
	{
		switch (m_format)
		{
			case RESULT_FORMAT_NEC:
				os << endl << endl << endl;
				break;
			
			default:
				os << endl << endl << endl;
				break;
		}
	}
	
	inline void int_out(int w, int i)
	{
		os << setw(w) << i;
	}
	
	inline void string_out(int w, char* s)
	{
		os << right << setw(w) << s;
	}
	
	inline void real_out(int w, int p, nec_float f, bool sci = true)
	{
		ios::fmtflags flags = ios::showpoint | ios::uppercase | ios::right;
		if (sci)
			flags |= ios::scientific;
		else
			flags |= ios::fixed;
		
		os.unsetf(ios::adjustfield | ios::basefield | ios::floatfield);
		os.setf(flags);
		os.precision(p);
		os.width(w);
		os << f;
	}
	
	inline void complex_out(int w, int p, nec_complex c, bool sci = true)
	{
		real_out(w,p,real(c),sci);
		separator();
		real_out(w,p,imag(c),sci);
	}

	inline void polar_out(int w, int p, nec_complex c, bool sci = true)
	{
		real_out(w,p,abs(c),sci);
		separator();
		real_out(w,p,arg_degrees(c),sci);
	}
	
};


/*!\brief Used to specify the kind of results we wish to pull out of the results database.
*/
enum nec_result_type
{
	RESULT_NORMALIZED_RECEIVING_PATTERN = 1,
	RESULT_STRUCTURE_EXCITATION = 2,
	RESULT_ANTENNA_INPUT = 3,
	RESULT_RADIATION_PATTERN = 4
};


/**
	This class contains the results of the NEC analysis. Set methods
	will store the results in this class, but will NOT print the results
	to a file
*/
class nec_base_result
{
private:
	bool _write_file;
	
protected:
	enum RESULT_FORMAT _result_format;
	
public:
	virtual void write_to_file(ostream& os) = 0;
	virtual enum nec_result_type get_result_type() = 0;
	
	nec_base_result()
		: _write_file(true), _result_format(RESULT_FORMAT_NEC)
	{
	}
	
	virtual ~nec_base_result()
	{
	}

	inline bool write_file() const
	{
		return _write_file;
	}
	
	inline void set_write_file(bool f)
	{
		_write_file = f;
	}

	inline void set_result_format(enum RESULT_FORMAT f)
	{
		_result_format = f;
	}
};


/** Normalized Receiving Pattern
*/
class nec_norm_rx_pattern : public nec_base_result
{
	// Receiving Pattern
	nec_float _norm_factor;
	nec_float _eta, _axial_ratio;
	int _segment_number;
	string _type;
	
	long n_theta;
	long n_phi;
	nec_float _theta0, _theta_step;
	nec_float _phi0, _phi_step;
	
	real_array _mag;
	
public:
	nec_norm_rx_pattern(
		int in_n_theta, int in_n_phi,
		real_array& in_mag,
		nec_float theta0, nec_float theta_step,
		nec_float phi0, nec_float phi_step,
		nec_float in_eta, 
		nec_float in_axial_ratio, 
		int in_segment_number, 
		string in_type)
	{
		n_theta = in_n_theta;
		n_phi = in_n_phi;
		
		_mag.copy(in_mag);
		_mag.resize(n_theta, n_phi);
		
		_theta0 = theta0;
		_theta_step = theta_step;
		
		_phi0 = phi0;
		_phi_step = phi_step;
		
		_eta = in_eta;
		_axial_ratio = in_axial_ratio;
		_segment_number = in_segment_number;
		_type = in_type;
		
		_mag.resize(n_theta, n_phi);
	}

	virtual ~nec_norm_rx_pattern()
	{
	}
	
	virtual enum nec_result_type get_result_type()
	{
		return RESULT_NORMALIZED_RECEIVING_PATTERN;
	}
		
	void set_input(int theta_index, int phi_index, nec_float mag)
	{
		_mag.set(theta_index,phi_index,mag);
	}
	
	nec_float get_mag(int theta_index, int phi_index)
	{
		return _mag.get(theta_index, phi_index);
	}
	
	nec_float get_norm_factor()
	{
		return _mag.max();
	}
	
	virtual void write_to_file(ostream& os)
	{
		if (n_theta == 0)
			return;
		if (n_phi == 0)
			return;
			
		nec_float norm_factor = get_norm_factor();
		
		output_helper oh(os,_result_format);
		
		oh.section_start();
		os << "                      ---- NORMALIZED RECEIVING PATTERN ----" << endl;
		os << "                      NORMALIZATION FACTOR: ";oh.real_out(11,4,norm_factor);os << endl;
		os << "                      ETA: ";oh.real_out(7,2,_eta,false); os << " DEGREES" << endl;
		os << "                      TYPE: " << _type << endl;
		os << "                      AXIAL RATIO: "; oh.real_out(6,3,_axial_ratio,false); os << endl;
		os << "                      SEGMENT No: ";oh.int_out( 5, _segment_number); os << endl << endl;
		os << "                      THETA     PHI       ---- PATTERN ----" << endl;
		os << "                      (DEG)    (DEG)       DB     MAGNITUDE" << endl;
		
		nec_float theta = _theta0;
		
		for (int t=0; t<n_theta; t++)
		{
			nec_float phi = _phi0;
			
			for (int p=0; p<n_phi;p++)
			{
				nec_float magnitude = _mag.get(t,p) / norm_factor;
				nec_float gain = db20(magnitude);
				
				oh.start_record();
				oh.padding("                    ");
				oh.real_out(7,2, theta, false);	oh.separator();
				oh.real_out(7,2, phi, false);	oh.separator();
				oh.padding("  "); oh.real_out(7,2, gain, false);	oh.separator();
				oh.padding(" "); oh.real_out(11,4, magnitude);
				oh.end_record();
				
				phi += _phi_step;
			}
			theta += _theta_step;
		}
	}
};



class structure_excitation_data
{
private:
	int m_segment_number, m_segment_tag;
	nec_complex m_voltage, m_current;
	nec_float m_power;
	
public:
	structure_excitation_data(int segment_number, int segment_tag, nec_complex voltage, nec_complex current, nec_float power)
	{
		m_segment_number = segment_number;
		m_segment_tag = segment_tag;
		m_voltage = voltage;
		m_current = current;
		m_power = power;
	}
	
	void write(output_helper& oh)
	{
		nec_complex admittance = m_current / m_voltage;
		nec_complex impedance = m_voltage / m_current;
		
/*		o.nec_printf(" %4d %5d %11.4E %11.4E %11.4E %11.4E %11.4E %11.4E %11.4E %11.4E %11.4E",
			segment_tag, m_segment_number, real(m_voltage), imag(m_voltage), real(m_current), imag(m_current),
			real(impedance), imag(impedance), real(admittance), imag(admittance), m_power);
*/			
		oh.start_record();
		oh.int_out(4, m_segment_tag);			oh.separator();
		oh.int_out(5, m_segment_number);		oh.separator();
		oh.complex_out(11,4, m_voltage);		oh.separator();
		oh.complex_out(11,4, m_current);		oh.separator();
		oh.complex_out(11,4, impedance);		oh.separator();
		oh.complex_out(11,4, admittance);		oh.separator();
		oh.real_out(11,4, m_power);
		oh.end_record();
	}
};

/*!\brief Holds structure excitation data at network connection points
*/
class nec_structure_excitation : public nec_base_result
{
private:
	std::vector<structure_excitation_data> m_data;
	long n_items;
	
public:

	nec_structure_excitation()
	{
		n_items = 0;
	}
	
	virtual ~nec_structure_excitation()	{ }
	
	
	virtual enum nec_result_type get_result_type()
	{
		return RESULT_STRUCTURE_EXCITATION;
	}
		
	void add(int segment_number, int segment_tag, nec_complex voltage, nec_complex current, nec_float power)
	{
		structure_excitation_data sed(segment_number, segment_tag, voltage, current, power);
		m_data.push_back(sed);
		n_items++;
	}
	
	virtual void write_to_file(ostream& os)
	{
		output_helper oh(os,_result_format);
		oh.section_start();
		os << "                          --------- STRUCTURE EXCITATION DATA AT NETWORK CONNECTION POINTS --------" << endl;
		os << "  TAG   SEG       VOLTAGE (VOLTS)          CURRENT (AMPS)         IMPEDANCE (OHMS)       ADMITTANCE (MHOS)     POWER" << endl;
		os << "  No:   No:     REAL      IMAGINARY     REAL      IMAGINARY     REAL      IMAGINARY     REAL      IMAGINARY   (WATTS)" << endl;
			
		for (int i = 0; i < n_items; i++ )
		{
			m_data[i].write(oh);
		}
	}	

};
		
/** Antenna Input Parameters 
*/
class nec_antenna_input : public nec_base_result
{
	// Antenna Input Parameters
	vector<int> _tag, _segment;
	vector<nec_float> _power;
	vector<nec_complex> _voltage, _current, _impedance, _admittance;
	long n_items;
	
public:
	nec_antenna_input()
	{
		n_items = 0;
	}

	virtual ~nec_antenna_input()
	{
	}
	
	virtual enum nec_result_type get_result_type()
	{
		return RESULT_ANTENNA_INPUT;
	}
	
	void set_input(int tag, int segment, nec_complex voltage, nec_complex current, nec_complex impedance, nec_complex admittance, nec_float power)
	{
		n_items++;
		_tag.push_back(tag);
		_segment.push_back(segment);
		_voltage.push_back(voltage);
		_current.push_back(current);
		_impedance.push_back(impedance);
		_admittance.push_back(admittance);
		_power.push_back(power);
	}
	
	virtual void write_to_file(ostream& os)
	{
		if (n_items == 0)
			return;
			
		output_helper oh(os,_result_format);
		oh.section_start();
		
		os << "                        --------- ANTENNA INPUT PARAMETERS ---------" << endl;	
		os << "  TAG   SEG       VOLTAGE (VOLTS)         CURRENT (AMPS)         IMPEDANCE (OHMS)        ADMITTANCE (MHOS)     POWER" << endl;
		os << "  NO.   NO.     REAL      IMAGINARY     REAL      IMAGINARY     REAL      IMAGINARY    REAL       IMAGINARY   (WATTS)" << endl;
		for (int i=0; i<n_items; i++)
		{
			oh.start_record();
			oh.int_out(4, _tag[i]);					oh.separator();
			oh.int_out(5, _segment[i]);				oh.separator();
			oh.complex_out(11,4, _voltage[i]);		oh.separator();
			oh.complex_out(11,4, _current[i]);		oh.separator();
			oh.complex_out(11,4, _impedance[i]);	oh.separator();
			oh.complex_out(11,4, _admittance[i]);	oh.separator();
			oh.real_out(11,4, _power[i]);
			oh.end_record();
		}
	}
};

class nec_radiation_pattern;

/**
	Stores a whole lot of nec_result objects. This class is effectively
	a database of the simulation results.
	
	Usage
	
		nec_antenna_input* ai = new nec_antenna_input();
		s_results.add(ai);
		ai->set_intput(tag, segment, voltage, current, impedance, admittance, power);
		ai->set_intput(tag, segment, voltage, current, impedance, admittance, power);
		ai->set_intput(tag, segment, voltage, current, impedance, admittance, power);
	
*/
class nec_results
{
	vector<nec_base_result*> _results;
	int _n;
	bool _file_out;
	
	
public:
	enum RESULT_FORMAT m_result_format;

	nec_results()
	{
		m_result_format = RESULT_FORMAT_NEC;
		
		_n = 0;
		_file_out = false;
	}

	// On destruction we write to a file.
	~nec_results()
	{
		// write_to_file();
		for (int i=0;i<_n;i++)
		{
			delete _results[i];
			_results[i] = NULL;
		}
	}
	
	void add(nec_base_result* br)
	{
		br->set_result_format(m_result_format);
		_results.push_back(br);
		_n++;
	}
	
	/*!\brief Get the nth result that matches the specified result type
		\param index The zero-based index for the result
		\param result_type The requested result type
		\return NULL if the result does not exist. 
		\note You must NOT delete the nec_norm_rx_pattern object when finished with it.
	*/
	nec_base_result* get_result(const long index, const enum nec_result_type result_type)
	{
		long counter = 0;
		
		for (int i=0;i<_n;i++)
		{
			if (_results[i]->get_result_type() == result_type)
			{
				if (index == counter++)
					return _results[i];
			}
		}
		
		return NULL;
	}
	
	/*!\brief Get normalized receiving pattern results
		\param index The zero-based index for the normalized receiving pattern.
		\return NULL if the result does not exist. 
		\note You must NOT delete the nec_norm_rx_pattern object when finished with it.
	*/
	nec_norm_rx_pattern* get_norm_rx_pattern(const long index)
	{
		return (nec_norm_rx_pattern*)get_result(index, RESULT_NORMALIZED_RECEIVING_PATTERN);
	}
	
	/*!\brief Get radiation pattern results
		\param index The zero-based index for the radiation pattern.
		\return NULL if the result does not exist. 
		\note You must NOT delete the nec_radiation_pattern object when finished with it.
	*/
	nec_radiation_pattern* get_radiation_pattern(const long index)
	{
		return (nec_radiation_pattern*)get_result(index, RESULT_RADIATION_PATTERN);
	}
	
	/*!\brief Get antenna input parameter results
		\param index The zero-based index for the antenna input.
		\return NULL if the result does not exist. 
		\note You must NOT delete the nec_antenna_input object when finished with it.
	*/
	nec_antenna_input* get_antenna_input(const long index)
	{
		return (nec_antenna_input*)get_result(index, RESULT_ANTENNA_INPUT);
	}

	/*!\brief Get structure excitation results
		\param index The zero-based index for the nec_structure_excitation.
		\return NULL if the result does not exist. 
		\note You must NOT delete the nec_structure_excitation object when finished with it.
	*/
	nec_structure_excitation* get_nec_structure_excitation(const long index)
	{
		return (nec_structure_excitation*)get_result(index, RESULT_STRUCTURE_EXCITATION);
	}
		
	
	void write_nec_file()
	{
		if (false == _file_out)
			return;
			
		for (int i=0;i<_n;i++)
		{
			if (_results[i]->write_file())
			{
				_results[i]->write_to_file(cout);
				_results[i]->set_write_file(false);
			}
		}
	}
	
	void set_stdout(bool f)
	{
		_file_out = f;
	}
};

#endif /* __nec_results__ */

