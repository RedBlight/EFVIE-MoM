#ifndef INC_WAVE_PLANE_INCLUDED
#define INC_WAVE_PLANE_INCLUDED

#include "IncWave.hpp"

template< class T >
class IncWave_Plane: public IncWave< T >
{
protected:
	vector< T > waveData_;

	T waveLength_;
	T waveNumber_;
	T amplitude_;
	T phaseOffset_;

	LUV::LuVector3< T > direction_;
	LUV::LuVector3< T > polarization_;
	LUV::LuVector3< T > phaseOrigin_;
	LUV::LuVector3< T > ampPolE_;
	LUV::LuVector3< T > ampPolM_;

	constexpr T pi_ = 3.141592653589793238462643383279502884L; // god dammit use boost next time bitch

	enum WaveDataIndex
	{
		WAVL,	// WaveLength, length of a full wave cycle, in meters.
		AMPL,	// Amplitude, 
		DIRX,	// Direction X, ongoing direction of the plane wave, must be unit vector.
		DIRY,	// Direction Y
		DIRZ,	// Direction Z
		POLX,	// Polarization X, polarization vector, must be otrhonormal to the direction vector.
		POLY,	// Polarization Y
		POLZ,	// Polarization Z
		PORX,	// PhaseOrigin X, phase origin is where the phase of the wave is at the value of PhaseOffset.
		PORY,	// PhaseOrigin Y
		PORZ,	// PhaseOrigin Z
		POFF	// PhaseOffset, or phase angle, between 0 and 2pi.
	};

	bool SetWaveData_( const vector< T >& waveData ) override
	{
		if( waveData.size() != 11 )
		{
			return false;
		}
		waveData_ = waveData;

		waveLength_	= waveData_[ WAVL ];
		waveNumber_ = 2.0 * pi_ / waveLength;
		amplitude_ = waveData_[ AMPL ];
		direction_ = LUV::LuVector3< T >( waveData_[ DIRX ], waveData_[ DIRY ], waveData_[ DIRZ ] );
		polarization_ = LUV::LuVector3< T >( waveData_[ POLX ], waveData_[ POLY ], waveData_[ POLZ ] );
		phaseOrigin_ = LUV::LuVector3< T >( waveData_[ PORX ], waveData_[ PORY ], waveData_[ PORZ ] );
		phaseOffset_ = waveData_[ POFF ];
		
		ampPolE_ = amplitude_ * polarization_;
		ampPolM_ = amplitude_ * polarization_; // modify this later

		return true;
	}

	LUV::LuVector3c< T > FieldE_( const LUV::LuVector3< T >& obsPoint ) const override
	{
		LUV::LuVector3< T > obsPointProj = LUV::ProjPlane( obsPoint, phaseOrigin_, direction_ );
		T dist = LUV::Dot( obsPoint - obsPointProj, direction_ );

		LUV::LuVector3c< T > fieldE = exp( 1i * ( phaseOffset_ - waveNumber_ * dist ) ) * ampPolE_;

		return fieldE;
	}

	LUV::LuVector3c< T > FieldM_( const LUV::LuVector3< T >& obsPoint ) const override
	{
		// !! make this correct later
		LUV::LuVector3< T > obsPointProj = LUV::ProjPlane( obsPoint, phaseOrigin_, direction_ );
		T dist = LUV::Dot( obsPoint - obsPointProj, direction_ );

		LUV::LuVector3c< T > fieldM = exp( 1i * ( phaseOffset_ - waveNumber_ * dist ) ) * ampPolM_;

		return fieldM;
	}

	PairEM< T > FieldEM_( const LUV::LuVector3< T >& obsPoint ) const override
	{
		// !! make this correct later
		LUV::LuVector3< T > obsPointProj = LUV::ProjPlane( obsPoint, phaseOrigin_, direction_ );
		T dist = LUV::Dot( obsPoint - obsPointProj, direction_ );
		
		LUV::LuVector3c< T > fieldE = exp( 1i * ( phaseOffset_ - waveNumber_ * dist ) ) * ampPolE_;
		LUV::LuVector3c< T > fieldM = exp( 1i * ( phaseOffset_ - waveNumber_ * dist ) ) * ampPolM_;

		return make_pair( fieldE, fieldM );
	}

public:
	
	IncWave_Plane()
	{

	}
	
	~IncWave_Plane()
	{

	}

};

#endif