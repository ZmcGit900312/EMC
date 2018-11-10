/*
	Copyright (C) 2004-2005  Timothy C.A. Molteno
	tim@molteno.net
	
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
#ifndef __nec_radiation_pattern__
#define __nec_radiation_pattern__

#include "nec_results.h"
#include "math_util.h"
#include "nec_ground.h"
#include "c_plot_card.h"

/** \brief The types of polarization that gain normalization should be done with
*/
enum polarization_norm
{	POL_MAJOR_AXIZ=1,
	POL_MINOR_AXIS=2,
	POL_VERTICAL=3,
	POL_HORIZONTAL=4,
	POL_TOTAL=5
};
		
class nec_context;

class nec_radiation_pattern : public nec_base_result
{
public:
	// Radiation Pattern
	nec_radiation_pattern(int in_n_theta, int in_n_phi,
		nec_float in_theta_start, nec_float in_phi_start,
		nec_float in_delta_theta, nec_float in_delta_phi,
		nec_float in_range,
		nec_ground& in_ground,
		int in_ifar, nec_float in_wavelength,
		nec_float pinr, nec_float pnlr,
		int in_rp_output_format, int in_rp_normalization, int in_rp_ipd, int in_rp_power_average,
		nec_float in_gnor,
		c_plot_card& in_plot_card);
		
	virtual void write_to_file(ostream& os)
	{
		write_to_file_aux(os);
	}

	virtual enum nec_result_type get_result_type()
	{
		return RESULT_RADIATION_PATTERN;
	}
		
	void analyze(nec_context* in_context);
	
	void write_gain_normalization()
	{
		if (_ifar != 1)
		{
			nec_float norm = get_maximum_gain_db();
			printf("Max Gain: %f\n",norm);
		}
	}
	
	nec_float get_maximum_gain_db()
	{
		return get_gain_normalization_factor(0);
	}
	
	/*! \brief Get the theta angle corresponding to the theta_index
	*/
	nec_float get_theta(int theta_index) const
	{
		return (m_theta_start + delta_theta*theta_index);
	}

	/*! \brief Get the number of theta angles
	*/
	int get_ntheta() const
	{
		return (n_theta);
	}

	/*! \brief Get the phi angle corresponding to the phi_index
	*/
	nec_float get_phi(int phi_index) const
	{
		return (m_phi_start + delta_phi*phi_index);
	}

	/*! \brief Get the number of phi angles
	*/
	int get_nphi() const
	{
		return (n_phi);
	}

	/*! \brief Get a total power gain from the radiation pattern
	*/
	nec_float get_power_gain(int theta_index, int phi_index) const
	{
		return _gain[get_index(theta_index, phi_index)];
	}

	/*! \brief Get  a power gain (vertical) from the radiation pattern
	*/
	nec_float get_power_gain_vert(int theta_index, int phi_index) const
	{
		return _power_gain_vert[get_index(theta_index, phi_index)];
	}

	/*! \brief Get  a power gain (horizontal) from the radiation pattern
	*/
	nec_float get_power_gain_horiz(int theta_index, int phi_index) const
	{
		return _power_gain_horiz[get_index(theta_index, phi_index)];
	}

private:

	int get_index(int theta_index, int phi_index) const;
	
	bool m_analysis_done;
	
	int n_theta, n_phi;
	
	nec_float m_theta_start, delta_theta;
	nec_float m_phi_start, delta_phi;
	
	int _ifar;
	
	nec_float m_range;
	
	nec_float _wavelength;
	nec_float _pinr;
	nec_float _pnlr;
	
	nec_float _average_power_gain;
	nec_float _average_power_solid_angle;
	nec_float _maximum_gain;

	int m_rp_normalization;
	int m_rp_output_format;
	int m_rp_power_average;
	int m_rp_ipd;
	
	nec_float m_rp_gnor;
	
	real_array	_gain;
	real_array	_power_gain_vert;
	real_array	_power_gain_horiz;
	real_array	_power_gain_tot;
	real_array	_polarization_axial_ratio;
	real_array	_polarization_tilt;
	int_array	_polarization_sense_index;
				
	complex_array	_e_theta;
	complex_array	_e_phi;
	complex_array	_e_r;
	
	void write_to_file_aux(ostream& os);
	
	nec_float get_gain_normalization_factor(nec_float gnor);
	
	void write_normalized_gain(ostream& os);
	
	nec_ground m_ground;
	c_plot_card m_plot_card;
};


#endif /* __nec_radiation_pattern__ */

