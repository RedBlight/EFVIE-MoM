function result = SingIntVol_Face( r_, vt_, v1_, v2_, v3_ )
	
	dirN = -M.PlaneNormalP( vt_, v1_, v2_, v3_ );
	vecRho = M.ProjPlane( r_, dirN, v1_ );
	magD = M.VecDot( r_ - vecRho, dirN );
	magAbsD = abs( magD );
	
	result = SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, v1_, v2_, v3_ );
	result = result + SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, v2_, v3_, v1_ );
	result = result + SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, v3_, v1_, v2_ );
	result = result * magD;
	
end

% 	inline void GreenVolVolIntegralFace(
% 		T& GIVV,
% 		LUV::LuVector3< T >& GDVV,
% 		const LUV::LuVector3< T >& vecObs,
% 		const LUV::LuVector3< T >& vecTetra,
% 		const LUV::LuVector3< T >& vecV1,
% 		const LUV::LuVector3< T >& vecV2,
% 		const LUV::LuVector3< T >& vecV3
% 	)
% 	{
% 		LUV::LuVector3< T > dirN = -LUV::PlaneNormalP( vecTetra, vecV1, vecV2, vecV3 );
% 		LUV::LuVector3< T > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
% 		T magD = LUV::Dot( vecObs - vecRho, dirN );
% 		T magAbsD = abs( magD );
% 		
% 		T edgeGIVV = 0;
% 		T edgeGDVV = 0;
% 		
% 		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
% 		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
% 		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
% 		
% 		GIVV += magD * edgeGIVV;
% 		GDVV += dirN * edgeGDVV;
% 	}