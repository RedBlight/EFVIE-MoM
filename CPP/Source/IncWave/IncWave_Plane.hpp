#ifndef INC_WAVE_PLANE_INCLUDED
#define INC_WAVE_PLANE_INCLUDED

#include <_BitDepthDefines.hpp>

#include <IncWave.hpp>

template< class T >
class IncWave_Plane: public IncWave< T >
{
protected:
	vector< T > waveData_;

	T waveLength_;
	T waveNumber_;
	T amplitudeU_;
	T amplitudeR_;
	T phaseOffsetU_;
	T phaseOffsetR_;

	LUV::LuVector3< T > direction_;
	LUV::LuVector3< T > polarizationU_;
	LUV::LuVector3< T > polarizationR_;
	LUV::LuVector3< T > phaseOrigin_;
	LUV::LuVector3< T > ampPolEU_;
	LUV::LuVector3< T > ampPolER_;
	LUV::LuVector3< T > ampPolMU_;
	LUV::LuVector3< T > ampPolMR_;

	static T pi_; // god dammit use boost next time bitch

	enum WaveDataIndex
	{
		WAVL,	// 0	WaveLength, length of a full wave cycle, in meters.
		DIRX,	// 1	Direction X, ongoing direction of the plane wave, must be unit vector.
		DIRY,	// 2	Direction Y
		DIRZ,	// 3	Direction Z
		POLX,	// 4	Polarization X, U polarization vector, must be otrhonormal to the direction vector.
		POLY,	// 5	Polarization Y, R polarization vector will be built from this and direction.
		POLZ,	// 6	Polarization Z
		PORX,	// 7	PhaseOrigin X, phase origin is where the phase of the wave is at the value of PhaseOffset.
		PORY,	// 8	PhaseOrigin Y
		PORZ,	// 9	PhaseOrigin Z
		POFU,	// 10	PhaseOffset U, or phase angle of U polarization, between 0 and 2pi.
		POFR,	// 11	PhaseOffset R
		AMPU,	// 12	Amplitude U, E field amplitude of U polarization
		AMPR	// 13	Amplitude R, 
	};

	bool SetWaveData_( const vector< T >& waveData ) override
	{
		if( waveData.size() != 14 )
		{
			return false;
		}
		waveData_ = waveData;

		waveLength_	= waveData_[ WAVL ];
		waveNumber_ = 2.0 * pi_ / waveLength_;
		amplitudeU_ = waveData_[ AMPU ];
		amplitudeR_ = waveData_[ AMPR ];
		phaseOffsetU_ = waveData_[ POFU ];
		phaseOffsetR_ = waveData_[ POFR ];

		direction_ = LUV::LuVector3< T >( waveData_[ DIRX ], waveData_[ DIRY ], waveData_[ DIRZ ] );
		polarizationU_ = LUV::LuVector3< T >( waveData_[ POLX ], waveData_[ POLY ], waveData_[ POLZ ] );
		polarizationR_ = LUV::Cross( direction_, polarizationU_ );
		LUV::Orthonormalize( direction, polarizationU_, polarizationR_ );

		phaseOrigin_ = LUV::LuVector3< T >( waveData_[ PORX ], waveData_[ PORY ], waveData_[ PORZ ] );
		
		ampPolEU_ = amplitudeU_ * polarizationU_;
		ampPolER_ = amplitudeR_ * polarizationR_;
		ampPolMU_ = amplitudeU_ * polarizationU_; // modify this later
		ampPolMR_ = amplitudeR_ * polarizationR_; // modify this later

		return true;
	}

	LUV::LuVector3c< T > FieldE_( const LUV::LuVector3< T >& obsPoint ) const override
	{
		LUV::LuVector3< T > obsPointProj = LUV::ProjPlane( obsPoint, phaseOrigin_, direction_ );
		T dist = LUV::Dot( obsPoint - obsPointProj, direction_ );
		T lambdaDist = waveNumber_ * dist;

		LUV::LuVector3c< T > fieldEU = exp( 1i * ( phaseOffsetU_ - lambdaDist ) ) * ampPolEU_;
		LUV::LuVector3c< T > fieldER = exp( 1i * ( phaseOffsetR_ - lambdaDist ) ) * ampPolER_;
		LUV::LuVector3c< T > fieldE = fieldEU + fieldER;

		return fieldE;
	}

	LUV::LuVector3c< T > FieldM_( const LUV::LuVector3< T >& obsPoint ) const override
	{
		// !! make this correct later
		LUV::LuVector3< T > obsPointProj = LUV::ProjPlane( obsPoint, phaseOrigin_, direction_ );
		T dist = LUV::Dot( obsPoint - obsPointProj, direction_ );
		T lambdaDist = waveNumber_ * dist;

		LUV::LuVector3c< T > fieldMU = exp( 1i * ( phaseOffsetU_ - lambdaDist ) ) * ampPolMU_;
		LUV::LuVector3c< T > fieldMR = exp( 1i * ( phaseOffsetR_ - lambdaDist ) ) * ampPolMR_;
		LUV::LuVector3c< T > fieldM = fieldMU + fieldMR;

		return fieldM;
	}

	PairEM< T > FieldEM_( const LUV::LuVector3< T >& obsPoint ) const override
	{
		// !! make this correct later
		LUV::LuVector3< T > obsPointProj = LUV::ProjPlane( obsPoint, phaseOrigin_, direction_ );
		T dist = LUV::Dot( obsPoint - obsPointProj, direction_ );
		T lambdaDist = waveNumber_ * dist;
		
		LUV::LuVector3c< T > fieldEU = exp( 1i * ( phaseOffsetU_ - lambdaDist ) ) * ampPolEU_;
		LUV::LuVector3c< T > fieldER = exp( 1i * ( phaseOffsetR_ - lambdaDist ) ) * ampPolER_;
		LUV::LuVector3c< T > fieldE = fieldEU + fieldER;

		LUV::LuVector3c< T > fieldMU = exp( 1i * ( phaseOffsetU_ - lambdaDist ) ) * ampPolMU_;
		LUV::LuVector3c< T > fieldMR = exp( 1i * ( phaseOffsetR_ - lambdaDist ) ) * ampPolMR_;
		LUV::LuVector3c< T > fieldM = fieldMU + fieldMR;

		return make_pair( fieldE, fieldM );
	}

	shared_ptr< IncWave< T > > Clone_() const override
	{
		return shared_ptr< IncWave< T > >( new IncWave_Plane< T >( *this ) );
	}

public:
	
	IncWave_Plane()
	{

	}
	
	~IncWave_Plane()
	{

	}

};


template< class T >
T IncWave_Plane< T >::pi_ = 3.141592653589793238462643383279502884L;

#endif