function result = SingIntSur( r_, v1_, v2_, v3_ )
	dirN = M.PlaneNormal( v1_, v2_, v3_ );
	vecRho = M.ProjPlane( r_, dirN, v1_ );
	magAbsD = abs( M.VecDot( r_ - vecRho, dirN ) );
	
	result = SingIntSur_Edge( dirN, vecRho, magAbsD, r_, v1_, v2_, v3_ );
	result = result + SingIntSur_Edge( dirN, vecRho, magAbsD, r_, v2_, v3_, v1_ );
	result = result + SingIntSur_Edge( dirN, vecRho, magAbsD, r_, v3_, v1_, v2_ );
end

