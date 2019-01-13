function result = SingIntSur_Edge( dirN, vecRho, magAbsD, r_, vf_, v1_, v2_ )
	
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
	
	magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
	magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
	magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );
	
	%validCheck = magP0 / M.VecLength( vecPM ) > 1E-15;
	if( magP0 / M.VecLength( vecM ) < 1E-7 )
		result = 0;
	else
		result = M.VecDot( dirP0, dirU ) * ( magP0 * magA3 - magAbsD * ( magA1 - magA2 ) );
	end
	
end

