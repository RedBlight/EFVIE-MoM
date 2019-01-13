classdef I
	%Singularity integrals
	
	properties( Constant )
		
	end
	
	methods( Static )
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%%% SUR
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
		function result = SingIntSur( r_, v1_, v2_, v3_ )
			
			dirN = M.PlaneNormal( v1_, v2_, v3_ );
			vecRho = M.ProjPlane( r_, dirN, v1_ );
			magAbsD = abs( M.VecDot( r_ - vecRho, dirN ) );

			result = I.SingIntSur_Edge( dirN, vecRho, magAbsD, r_, v1_, v2_, v3_ );
			result = result + I.SingIntSur_Edge( dirN, vecRho, magAbsD, r_, v2_, v3_, v1_ );
			result = result + I.SingIntSur_Edge( dirN, vecRho, magAbsD, r_, v3_, v1_, v2_ );
			
		end
		
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

			if( magP0 / M.VecLength( vecM ) < 1E-7 )
				result = 0;
			else
				result = M.VecDot( dirP0, dirU ) * ( magP0 * magA3 - magAbsD * ( magA1 - magA2 ) );
			end

		end
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
		
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%%% VOL
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
		function result = SingIntVol( r_, v1_, v2_, v3_, v4_ )
			
			result = I.SingIntVol_Face( r_, v1_, v2_, v3_, v4_ );
			result = result + I.SingIntVol_Face( r_, v2_, v3_, v4_, v1_ );
			result = result + I.SingIntVol_Face( r_, v3_, v4_, v1_, v2_ );
			result = result + I.SingIntVol_Face( r_, v4_, v1_, v2_, v3_ );
			result = result / 2;
			
		end
		
		function result = SingIntVol_Face( r_, vt_, v1_, v2_, v3_ )
	
			dirN = -M.PlaneNormalP( vt_, v1_, v2_, v3_ );
			vecRho = M.ProjPlane( r_, dirN, v1_ );
			magD = M.VecDot( r_ - vecRho, dirN );
			magAbsD = abs( magD );

			result = I.SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, v1_, v2_, v3_ );
			result = result + I.SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, v2_, v3_, v1_ );
			result = result + I.SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, v3_, v1_, v2_ );
			result = result * magD;

		end
		
		function result = SingIntVol_Edge( dirN, vecRho, magD, r_, vt_, vf_, v1_, v2_ )
		
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

			if( magP0 / M.VecLength( vecM ) < 1E-7 )
				result = 0;
			else
				result = magA6 * ( magAbsD * magA7 - magP0 * magA3 );
			end

		end
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
		
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%%% VEC
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
		function result = SingIntVec( r_, v1_, v2_, v3_, v4_ )
	
			result = I.SingIntVec_Face( r_, v1_, v2_, v3_, v4_ );
			result = result + I.SingIntVec_Face( r_, v2_, v3_, v4_, v1_ );
			result = result + I.SingIntVec_Face( r_, v3_, v4_, v1_, v2_ );
			result = result + I.SingIntVec_Face( r_, v4_, v1_, v2_, v3_ );
			result = result ./ 3;

		end
		
		function result = SingIntVec_Face( r_, vt_, v1_, v2_, v3_ )
	
			dirN = -M.PlaneNormalP( vt_, v1_, v2_, v3_ );
			vecRho = M.ProjPlane( r_, dirN, v1_ );
			magD = M.VecDot( r_ - vecRho, dirN );
			magAbsD = abs( magD );

			result = I.SingIntVec_Edge( dirN, vecRho, magD, r_, vt_, v1_, v2_, v3_ );
			result = result + I.SingIntVec_Edge( dirN, vecRho, magD, r_, vt_, v2_, v3_, v1_ );
			result = result + I.SingIntVec_Edge( dirN, vecRho, magD, r_, vt_, v3_, v1_, v2_ );
			result = result .* dirN;

		end
		
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

			if( magP0 / M.VecLength( vecM ) < 1E-7 )
				result = 0;
			else
				result = magA6 * ( magA3 * magA4 + magA5 - magAbsDC * magA7 );
			end

		end
		
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
	end
end

