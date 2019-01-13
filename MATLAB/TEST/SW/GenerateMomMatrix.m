function [ momMatrix ] = GenerateMomMatrix( ...
	lastSingleBaseId, faceCount, angFreq, vertexList, ...
	tetraVertexIndex, faceTetraIndex, swgVertexIndex, ...
	epsRel, faceArea, tetraVol, tetraMidPoint, ...
	kontrast, kontrastDiff, relSS, ...
	I2, I3TT, I3TS, I3ST, I3SS ...
)
	% m = which row, n = which col
	
	momMatrix = zeros( faceCount, faceCount );
	
	K2 = - angFreq * angFreq * M.mu0 / ( 36 * pi );
	K3 = - 1 / ( 4 * pi * M.eps0 );
	SL = [ 1, -1 ];
	
	doubleBaseStartId = lastSingleBaseId + 1;
	
	for idFaceM = 1 : lastSingleBaseId
		
		idTetraM1 = faceTetraIndex( 1, idFaceM );

		SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );

		MC1 = tetraMidPoint( :, idTetraM1 );

		AM = faceArea( idFaceM );
		
		parfor idFaceN = 1 : lastSingleBaseId
			
			idTetraN1 = faceTetraIndex( 1, idFaceN );
			
			SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );

			AN = faceArea( idFaceN );

			KN1 = kontrast( idTetraN1 );
			KDN = kontrastDiff( idFaceN );

			VN1 = tetraVol( idTetraN1 );
			
			%%%
			
			mt1 = 0;
			if( relSS( idFaceM, idFaceN ) )
				a = 1;
				b = 1;
				tetraIdxA = faceTetraIndex( a, idFaceM );
				tetraIdxB = faceTetraIndex( b, idFaceN );
				if( tetraIdxA == tetraIdxB )

					epsFactor = 1 / ( M.eps0 * epsRel( tetraIdxB ) );
					geomFactor = faceArea( idFaceM ) * faceArea( idFaceN ) / ( 9 * tetraVol( tetraIdxA ) );
					v1 = vertexList( :, tetraVertexIndex( 1, tetraIdxA ) );
					v2 = vertexList( :, tetraVertexIndex( 2, tetraIdxA ) );
					v3 = vertexList( :, tetraVertexIndex( 3, tetraIdxA ) );
					v4 = vertexList( :, tetraVertexIndex( 4, tetraIdxA ) );
					vm = vertexList( :, swgVertexIndex( a, idFaceM ) );
					vn = vertexList( :, swgVertexIndex( b, idFaceN ) );
					vc = tetraMidPoint( :, tetraIdxA );
					gm = SL( a ) * SL( b );

					mt1 = mt1 + gm * epsFactor * geomFactor * ( ...
						(8/5) * M.VecLength( vc )^2 - M.VecDot( vm + vn, vc ) + M.VecDot( vm, vn ) ...
						- (1/10) * ( M.VecDot( v1, v2 ) + M.VecDot( v1, v3 ) + M.VecDot( v1, v4 ) ...
						+ M.VecDot( v2, v3 ) + M.VecDot( v2, v4 ) + M.VecDot( v3, v4 ) ) ...
					);

				end
			end
			
			%%%
		
			K = K2 * AN * AM;

			CN1 = KN1 / VN1;

			I3TT11 = I3TT( idTetraM1, idTetraN1 );

			I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT11;

			mt2 = K * ( ...
				+ M.VecDot( MC1 - SM1, CN1 .* I_11 ) ...
			);
		
			%%%

			C1 = K3 * AN;
			C2 = KN1 / VN1;
			C4 = - KDN / AN;

			phi1 = C1 * ( ...
				C2 * I3TT11 ...
				+ C4 * I3TS( idTetraM1, idFaceN ) ...
			);

			phi2 = C1 * ( ...
				C2 * I3ST( idFaceM, idTetraN1 ) ...
				+ C4 * I3SS( idFaceM, idFaceN ) ...
			);

			mt3 = AN * ( phi2 - phi1 );
			
			%%%
			
			mt = mt1 + mt2 + mt3;
			momMatrix( idFaceM, idFaceN ) = mt;
			
		end
		parfor idFaceN = doubleBaseStartId : faceCount
			
			idTetraN1 = faceTetraIndex( 1, idFaceN );
			idTetraN2 = faceTetraIndex( 2, idFaceN );
			
			SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
			SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );

			AN = faceArea( idFaceN );

			KN1 = kontrast( idTetraN1 );
			KN2 = kontrast( idTetraN2 );
			KDN = kontrastDiff( idFaceN );

			VN1 = tetraVol( idTetraN1 );
			VN2 = tetraVol( idTetraN2 );
			
			%%%
			
			mt1 = 0;
			if( relSS( idFaceM, idFaceN ) )
				a = 1;
				for b = 1 : 2
					tetraIdxA = faceTetraIndex( a, idFaceM );
					tetraIdxB = faceTetraIndex( b, idFaceN );
					if( tetraIdxA == tetraIdxB )

						epsFactor = 1 / ( M.eps0 * epsRel( tetraIdxB ) );
						geomFactor = faceArea( idFaceM ) * faceArea( idFaceN ) / ( 9 * tetraVol( tetraIdxA ) );
						v1 = vertexList( :, tetraVertexIndex( 1, tetraIdxA ) );
						v2 = vertexList( :, tetraVertexIndex( 2, tetraIdxA ) );
						v3 = vertexList( :, tetraVertexIndex( 3, tetraIdxA ) );
						v4 = vertexList( :, tetraVertexIndex( 4, tetraIdxA ) );
						vm = vertexList( :, swgVertexIndex( a, idFaceM ) );
						vn = vertexList( :, swgVertexIndex( b, idFaceN ) );
						vc = tetraMidPoint( :, tetraIdxA );
						gm = SL( a ) * SL( b );

						mt1 = mt1 + gm * epsFactor * geomFactor * ( ...
							(8/5) * M.VecLength( vc )^2 - M.VecDot( vm + vn, vc ) + M.VecDot( vm, vn ) ...
							- (1/10) * ( M.VecDot( v1, v2 ) + M.VecDot( v1, v3 ) + M.VecDot( v1, v4 ) ...
							+ M.VecDot( v2, v3 ) + M.VecDot( v2, v4 ) + M.VecDot( v3, v4 ) ) ...
						);

					end
				end
			end
			
			%%%

			K = K2 * AN * AM;

			CN1 = KN1 / VN1;
			CN2 = - KN2 / VN2;

			I3TT11 = I3TT( idTetraM1, idTetraN1 );
			I3TT12 = I3TT( idTetraM1, idTetraN2 );

			I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT11;
			I_12 = I2( :, idTetraM1, idTetraN2 ) + ( MC1 - SN2 ) .* I3TT12;

			mt2 = K * ( ...
				+ M.VecDot( MC1 - SM1, CN1 .* I_11 + CN2 .* I_12 ) ...
			);
		
			%%%

			C1 = K3 * AN;
			C2 = KN1 / VN1;
			C3 = - KN2 / VN2;
			C4 = - KDN / AN;
			
			phi1 = C1 * ( ...
				C2 * I3TT11 ...
				+ C3 * I3TT12 ...
				+ C4 * I3TS( idTetraM1, idFaceN ) ...
			);

			phi2 = C1 * ( ...
				C2 * I3ST( idFaceM, idTetraN1 ) ...
				+ C3 * I3ST( idFaceM, idTetraN2 ) ...
				+ C4 * I3SS( idFaceM, idFaceN ) ...
			);

			mt3 = AN * ( phi2 - phi1 );
			
			%%%
			
			mt = mt1 + mt2 + mt3;
			momMatrix( idFaceM, idFaceN ) = mt;
			
		end
	end
	
	
	
	for idFaceM = doubleBaseStartId : faceCount
		
		idTetraM1 = faceTetraIndex( 1, idFaceM );
		idTetraM2 = faceTetraIndex( 2, idFaceM );

		SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
		SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );

		MC1 = tetraMidPoint( :, idTetraM1 );
		MC2 = tetraMidPoint( :, idTetraM2 );

		AM = faceArea( idFaceM );
		
		parfor idFaceN = 1 : lastSingleBaseId
			
			idTetraN1 = faceTetraIndex( 1, idFaceN );
			
			SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );

			AN = faceArea( idFaceN );

			KN1 = kontrast( idTetraN1 );
			KDN = kontrastDiff( idFaceN );

			VN1 = tetraVol( idTetraN1 );
			
			%%%
			
			mt1 = 0;
			if( relSS( idFaceM, idFaceN ) )
				for a = 1 : 2
					b = 1;
					tetraIdxA = faceTetraIndex( a, idFaceM );
					tetraIdxB = faceTetraIndex( b, idFaceN );
					if( tetraIdxA == tetraIdxB )

						epsFactor = 1 / ( M.eps0 * epsRel( tetraIdxB ) );
						geomFactor = faceArea( idFaceM ) * faceArea( idFaceN ) / ( 9 * tetraVol( tetraIdxA ) );
						v1 = vertexList( :, tetraVertexIndex( 1, tetraIdxA ) );
						v2 = vertexList( :, tetraVertexIndex( 2, tetraIdxA ) );
						v3 = vertexList( :, tetraVertexIndex( 3, tetraIdxA ) );
						v4 = vertexList( :, tetraVertexIndex( 4, tetraIdxA ) );
						vm = vertexList( :, swgVertexIndex( a, idFaceM ) );
						vn = vertexList( :, swgVertexIndex( b, idFaceN ) );
						vc = tetraMidPoint( :, tetraIdxA );
						gm = SL( a ) * SL( b );

						mt1 = mt1 + gm * epsFactor * geomFactor * ( ...
							(8/5) * M.VecLength( vc )^2 - M.VecDot( vm + vn, vc ) + M.VecDot( vm, vn ) ...
							- (1/10) * ( M.VecDot( v1, v2 ) + M.VecDot( v1, v3 ) + M.VecDot( v1, v4 ) ...
							+ M.VecDot( v2, v3 ) + M.VecDot( v2, v4 ) + M.VecDot( v3, v4 ) ) ...
						);

					end
				end
			end
			
			%%%

			K = K2 * AN * AM;

			CN1 = KN1 / VN1;

			I3TT11 = I3TT( idTetraM1, idTetraN1 );
			I3TT21 = I3TT( idTetraM2, idTetraN1 );

			I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT11;
			I_21 = I2( :, idTetraM2, idTetraN1 ) + ( MC2 - SN1 ) .* I3TT21;

			mt2 = K * ( ...
				+ M.VecDot( MC1 - SM1, CN1 .* I_11 ) ...
				- M.VecDot( MC2 - SM2, CN1 .* I_21 ) ...
			);
		
			%%%

			C1 = K3 * AN;
			C2 = KN1 / VN1;
			C4 = - KDN / AN;
				
			phi1 = C1 * ( ...
				C2 * I3TT11 ...
				+ C4 * I3TS( idTetraM1, idFaceN ) ...
			);

			phi2 = C1 * ( ...
				C2 * I3TT21 ...
				+ C4 * I3TS( idTetraM2, idFaceN ) ...
			);

			mt3 = AN * ( phi2 - phi1 );
			
			%%%
			
			mt = mt1 + mt2 + mt3;
			momMatrix( idFaceM, idFaceN ) = mt;
			
		end
		
		parfor idFaceN = doubleBaseStartId : faceCount
			
			idTetraN1 = faceTetraIndex( 1, idFaceN );
			idTetraN2 = faceTetraIndex( 2, idFaceN );
			
			SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
			SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );

			AN = faceArea( idFaceN );

			KN1 = kontrast( idTetraN1 );
			KN2 = kontrast( idTetraN2 );
			KDN = kontrastDiff( idFaceN );

			VN1 = tetraVol( idTetraN1 );
			VN2 = tetraVol( idTetraN2 );
			
			%%%
			
			mt1 = 0;
			if( relSS( idFaceM, idFaceN ) )
				for a = 1 : 2
					for b = 1 : 2
						tetraIdxA = faceTetraIndex( a, idFaceM );
						tetraIdxB = faceTetraIndex( b, idFaceN );
						if( tetraIdxA == tetraIdxB )

							epsFactor = 1 / ( M.eps0 * epsRel( tetraIdxB ) );
							geomFactor = faceArea( idFaceM ) * faceArea( idFaceN ) / ( 9 * tetraVol( tetraIdxA ) );
							v1 = vertexList( :, tetraVertexIndex( 1, tetraIdxA ) );
							v2 = vertexList( :, tetraVertexIndex( 2, tetraIdxA ) );
							v3 = vertexList( :, tetraVertexIndex( 3, tetraIdxA ) );
							v4 = vertexList( :, tetraVertexIndex( 4, tetraIdxA ) );
							vm = vertexList( :, swgVertexIndex( a, idFaceM ) );
							vn = vertexList( :, swgVertexIndex( b, idFaceN ) );
							vc = tetraMidPoint( :, tetraIdxA );
							gm = SL( a ) * SL( b );

							mt1 = mt1 + gm * epsFactor * geomFactor * ( ...
								(8/5) * M.VecLength( vc )^2 - M.VecDot( vm + vn, vc ) + M.VecDot( vm, vn ) ...
								- (1/10) * ( M.VecDot( v1, v2 ) + M.VecDot( v1, v3 ) + M.VecDot( v1, v4 ) ...
								+ M.VecDot( v2, v3 ) + M.VecDot( v2, v4 ) + M.VecDot( v3, v4 ) ) ...
							);

						end
					end
				end
			end
			
			%%%

			K = K2 * AN * AM;

			CN1 = KN1 / VN1;
			CN2 = - KN2 / VN2;

			I3TT11 = I3TT( idTetraM1, idTetraN1 );
			I3TT12 = I3TT( idTetraM1, idTetraN2 );
			I3TT21 = I3TT( idTetraM2, idTetraN1 );
			I3TT22 = I3TT( idTetraM2, idTetraN2 );

			I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT11;
			I_12 = I2( :, idTetraM1, idTetraN2 ) + ( MC1 - SN2 ) .* I3TT12;
			I_21 = I2( :, idTetraM2, idTetraN1 ) + ( MC2 - SN1 ) .* I3TT21;
			I_22 = I2( :, idTetraM2, idTetraN2 ) + ( MC2 - SN2 ) .* I3TT22;

			mt2 = K * ( ...
				+ M.VecDot( MC1 - SM1, CN1 .* I_11 + CN2 .* I_12 ) ...
				- M.VecDot( MC2 - SM2, CN1 .* I_21 + CN2 .* I_22 ) ...
			);
		
			%%%

			C1 = K3 * AN;
			C2 = KN1 / VN1;
			C3 = - KN2 / VN2;
			C4 = - KDN / AN;

			phi1 = C1 * ( ...
				C2 * I3TT11 ...
				+ C3 * I3TT12 ...
				+ C4 * I3TS( idTetraM1, idFaceN ) ...
			);

			phi2 = C1 * ( ...
				C2 * I3TT21 ...
				+ C3 * I3TT22 ...
				+ C4 * I3TS( idTetraM2, idFaceN ) ...
			);

			mt3 = AN * ( phi2 - phi1 );
			
			%%%
			
			mt = mt1 + mt2 + mt3;
			momMatrix( idFaceM, idFaceN ) = mt;
			
		end
	end
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
% 	
% 	
% 	
% 	
% 	parfor idFaceM = 1 : faceCount
% 		for idFaceN = 1 : faceCount
% 			
% 			idTetraM1 = faceTetraIndex( 1, idFaceM );
% 			idTetraM2 = faceTetraIndex( 2, idFaceM );
% 			
% 			idTetraN1 = faceTetraIndex( 1, idFaceN );
% 			idTetraN2 = faceTetraIndex( 2, idFaceN );
% 			
% 			isSingleM = idTetraM2 == -1;
% 			isSingleN = idTetraN2 == -1;
% 			
% 			if( ~isSingleM && ~isSingleN )
% 				
% 				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
% 				SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );
% 				
% 				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
% 				SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
% 				
% 				MC1 = tetraMidPoint( :, idTetraM1 );
% 				MC2 = tetraMidPoint( :, idTetraM2 );
% 				
% 				AN = faceArea( idFaceN );
% 				AM = faceArea( idFaceM );
% 				
% 				KN1 = kontrast( idTetraN1 );
% 				KN2 = kontrast( idTetraN2 );
% 				KDN = kontrastDiff( idFaceN );
% 				
% 				VN1 = tetraVol( idTetraN1 );
% 				VN2 = tetraVol( idTetraN2 );
% 				
% 				K = K2 * AN * AM;
% 				
% 				CN1 = KN1 / VN1;
% 				CN2 = - KN2 / VN2;
% 				
% 				I3TT11 = I3TT( idTetraM1, idTetraN1 );
% 				I3TT12 = I3TT( idTetraM1, idTetraN2 );
% 				I3TT21 = I3TT( idTetraM2, idTetraN1 );
% 				I3TT22 = I3TT( idTetraM2, idTetraN2 );
% 				
% 				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT11;
% 				I_12 = I2( :, idTetraM1, idTetraN2 ) + ( MC1 - SN2 ) .* I3TT12;
% 				I_21 = I2( :, idTetraM2, idTetraN1 ) + ( MC2 - SN1 ) .* I3TT21;
% 				I_22 = I2( :, idTetraM2, idTetraN2 ) + ( MC2 - SN2 ) .* I3TT22;
% 				
% 				mt2 = K * ( ...
% 					+ M.VecDot( MC1 - SM1, CN1 .* I_11 + CN2 .* I_12 ) ...
% 					- M.VecDot( MC2 - SM2, CN1 .* I_21 + CN2 .* I_22 ) ...
% 				);
% 				
% 				C1 = K3 * AN;
% 				C2 = KN1 / VN1;
% 				C3 = - KN2 / VN2;
% 				C4 = - KDN / AN;
% 				
% 				phi1 = C1 * ( ...
% 					C2 * I3TT11 ...
% 					+ C3 * I3TT12 ...
% 					+ C4 * I3TS( idTetraM1, idFaceN ) ...
% 				);
% 				
% 				phi2 = C1 * ( ...
% 					C2 * I3TT21 ...
% 					+ C3 * I3TT22 ...
% 					+ C4 * I3TS( idTetraM2, idFaceN ) ...
% 				);
% 			
% 				mt3 = faceArea( idFaceM ) * ( phi2 - phi1 );
% 				
% 			elseif( ~isSingleM && isSingleN )
% 				
% 				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
% 				
% 				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
% 				SM2 = vertexList( :, swgVertexIndex( 2, idFaceM ) );
% 				
% 				MC1 = tetraMidPoint( :, idTetraM1 );
% 				MC2 = tetraMidPoint( :, idTetraM2 );
% 				
% 				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
% 				
% 				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
% 				
% 				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
% 				I_21 = I2( :, idTetraM2, idTetraN1 ) + ( MC2 - SN1 ) .* I3TT( idTetraM2, idTetraN1 );
% 				
% 				mt2 = K * ( ...
% 					+ M.VecDot( MC1 - SM1, CN1 .* I_11 ) ...
% 					- M.VecDot( MC2 - SM2, CN1 .* I_21 ) ...
% 				);
% 					
% 			elseif( isSingleM && ~isSingleN )
% 				
% 				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
% 				SN2 = vertexList( :, swgVertexIndex( 2, idFaceN ) );
% 				
% 				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
% 				
% 				MC1 = tetraMidPoint( :, idTetraM1 );
% 				
% 				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
% 				
% 				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
% 				CN2 = - kontrast( idTetraN2 ) / tetraVol( idTetraN2 );
% 				
% 				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
% 				I_12 = I2( :, idTetraM1, idTetraN2 ) + ( MC1 - SN2 ) .* I3TT( idTetraM1, idTetraN2 );
% 				
% 				mt2 = K * ( ...
% 					+ M.VecDot( MC1 - SM1, CN1 .* I_11 + CN2 .* I_12 ) ...
% 				);
% 					
% 			elseif( isSingleM && isSingleN )
% 				
% 				SN1 = vertexList( :, swgVertexIndex( 1, idFaceN ) );
% 				
% 				SM1 = vertexList( :, swgVertexIndex( 1, idFaceM ) );
% 				
% 				MC1 = tetraMidPoint( :, idTetraM1 );
% 				
% 				K = - angFreq * angFreq * M.mu0 * faceArea( idFaceN ) * faceArea( idFaceM ) / ( 36 * pi );
% 				
% 				CN1 = kontrast( idTetraN1 ) / tetraVol( idTetraN1 );
% 				
% 				I_11 = I2( :, idTetraM1, idTetraN1 ) + ( MC1 - SN1 ) .* I3TT( idTetraM1, idTetraN1 );
% 				
% 				mt2 = K * ( ...
% 					+ M.VecDot( MC1 - SM1, CN1 .* I_11 ) ...
% 				);
% 				
% 			end
% 			
% 			momTerm2( idFaceM, idFaceN ) = mt2;
% 			
% 		end
% 	end
	
	
end

