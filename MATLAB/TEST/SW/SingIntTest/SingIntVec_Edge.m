function result = SingIntVec_Edge( dirN, vecRho, magD, r_, vt_, vf_, v1_, v2_ )
	
	dirU = -M.LineNormalP( v1_, v2_, vf_ );
	dirL = M.VecCross( dirN, dirU );
	
	vecS = v2_ + v1_;
	vecM = v2_ - v1_;
	dirM = M.VecUnit( vecM );
	vecB = M.VecDot( dirM, dirL ) .* vecM;
	vecRhoM = ( vecS - vecB ) ./ 2;
	vecRhoP = ( vecS + vecB ) ./ 2;
	
	vecPM = vecRhoM - vecRho;
	vecPP = vecRhoP - vecRho;
	
	magLM = M.VecDot( vecPM, dirL );
	magLP = M.VecDot( vecPP, dirL );
	magP0 = abs( M.VecDot( vecPM, dirU ) );
	
	dirP0 = ( vecPM - magLM .* dirL ) ./ magP0;
	
	magR0 = M.VecLength( r_ - M.ProjLine( vecRhoM, vecRhoP, r_ ) );
	magRM = M.VecLength( r_ - vecRhoM );
	magRP = M.VecLength( r_ - vecRhoP );
	
	magR0S = magR0 * magR0;
	magAbsD = abs( magD );
	magDS = magD * magD;
	magAbsDC = magAbsD * magDS;
	
	magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
	magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
	magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );
	magA4 = ( magP0 * ( magR0S + 2.0 * magDS ) ) / 2.0;
	magA5 = ( magP0 * ( magLP * magRP - magLM * magRM ) ) / 2.0;
	
	magA6 = M.VecDot( dirP0, dirU );
	magA7 = magA1 - magA2;
	
	%validCheck = magP0 / M.VecLength( vecPM ) > 1E-15;
	if( magP0 / M.VecLength( vecM ) < 1E-7 )
		result = 0;
	else
		result = magA6 * ( magA3 * magA4 + magA5 - magAbsDC * magA7 );
	end
	
end

% 	inline void GreenVolVolIntegralEdge(
% 		T& edgeGIVV,
% 		T& edgeGDVV,
% 		const LUV::LuVector3< T >& dirN,
% 		const LUV::LuVector3< T >& vecRho,
% 		const T& magD,
% 		const LUV::LuVector3< T >& vecObs,
% 		const LUV::LuVector3< T >& vecTetra,
% 		const LUV::LuVector3< T >& vecFace,
% 		const LUV::LuVector3< T >& vecV1,
% 		const LUV::LuVector3< T >& vecV2
% 	)
% 	{
% 		LUV::LuVector3< T > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
% 		LUV::LuVector3< T > dirL = LUV::Cross( dirN, dirU );
% 
% 		LUV::LuVector3< T > vecS = vecV2 + vecV1;
% 		LUV::LuVector3< T > vecM = vecV2 - vecV1;
% 		LUV::LuVector3< T > dirM = Unit( vecM );
% 		LUV::LuVector3< T > vecB = LUV::Dot( dirM, dirL ) * vecM;
% 		LUV::LuVector3< T > vecRhoM = ( vecS - vecB ) / 2;
% 		LUV::LuVector3< T > vecRhoP = ( vecS + vecB ) / 2;
% 		
% 		LUV::LuVector3< T > vecPM = vecRhoM - vecRho;
% 		LUV::LuVector3< T > vecPP = vecRhoP - vecRho;
% 
% 		T magLM = LUV::Dot( vecPM, dirL );
% 		T magLP = LUV::Dot( vecPP, dirL );
% 		T magP0 = abs( LUV::Dot( vecPM, dirU ) );
% 
% 		LUV::LuVector3< T > dirP0 = ( vecPM - magLM * dirL ) / magP0;
% 		
% 		T magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
% 		T magRM = LUV::Length( vecObs - vecRhoM );
% 		T magRP = LUV::Length( vecObs - vecRhoP );
% 
% 		T magR0S = magR0 * magR0;
% 		T magAbsD = abs( magD );
% 		T magDS = magD * magD;
% 		T magAbsDC = magAbsD * magDS;
% 
% 		T magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
% 		T magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
% 		T magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );
% 		T magA4 = ( magP0 * ( magR0S + 2.0 * magDS ) ) / 2.0;
% 		T magA5 = ( magP0 * ( magLP * magRP - magLM * magRM ) ) / 2.0;
% 
% 		T magA6 = LUV::Dot( dirP0, dirU );
% 		T magA7 = magA1 - magA2;
% 
% 		
% 		if( ( magP0 / LUV::Length( vecPM ) < 1E-12 ) ){ cout << ">>>>NaN @ VolVol" << endl; char stopper; cin >> stopper; };
% 
% 		edgeGIVV += ( magP0 / LUV::Length( vecPM ) < 1E-12 ) ? 0 : magA6 * ( magAbsD * magA7 - magP0 * magA3 );
% 		edgeGDVV += ( magP0 / LUV::Length( vecPM ) < 1E-12 ) ? 0 : magA6 * ( magA3 * magA4 + magA5 - magAbsDC * magA7 );
% 	}