function result = AdaptiveSingIntVec( vr, v1, v2, v3, v4, level, valPrev )

	integrand = @( vr, vi ) ( vi - vr ) ./ M.VecLength( vi - vr );
	
	if( level > 6 )
		s = sum( valPrev );
		if( isinf( s ) || isnan( s ) )
			result = [ 0; 0; 0 ];
		else
			result = valPrev;
		end
	else
		v12 = ( v1 + v2 ) ./ 2;
		v13 = ( v1 + v3 ) ./ 2;
		v14 = ( v1 + v4 ) ./ 2;
		v23 = ( v2 + v3 ) ./ 2;
		v24 = ( v2 + v4 ) ./ 2;
		v34 = ( v3 + v4 ) ./ 2;
		
% 		v1, v12, v13, v14
% 		v2, v12, v23, v24
% 		v3, v13, v23, v34
% 		v4, v14, v24, v34
% 		v12, v13, v14, v23
% 		v12, v14, v23, v24
% 		v13, v14, v23, v34
% 		v14, v23, v24, v34
		
		val1 = M.TetVol( v1, v12, v13, v14 ) .* integrand( vr, M.TetMid( v1, v12, v13, v14 ) );
		val2 = M.TetVol( v2, v12, v23, v24 ) .* integrand( vr, M.TetMid( v2, v12, v23, v24 ) );
		val3 = M.TetVol( v3, v13, v23, v34 ) .* integrand( vr, M.TetMid( v3, v13, v23, v34 ) );
		val4 = M.TetVol( v4, v14, v24, v34 ) .* integrand( vr, M.TetMid( v4, v14, v24, v34 ) );
		val5 = M.TetVol( v12, v13, v14, v23 ) .* integrand( vr, M.TetMid( v12, v13, v14, v23 ) );
		val6 = M.TetVol( v12, v14, v23, v24 ) .* integrand( vr, M.TetMid( v12, v14, v23, v24 ) );
		val7 = M.TetVol( v13, v14, v23, v34 ) .* integrand( vr, M.TetMid( v13, v14, v23, v34 ) );
		val8 = M.TetVol( v14, v23, v24, v34 ) .* integrand( vr, M.TetMid( v14, v23, v24, v34 ) );
		
		val = val1 + val2 + val3 + val4 + val5 + val6 + val7 + val8;
		
		errRel = max( [ ...
			abs( ( val(1) - valPrev(1) ) ./ valPrev(1) ); ...
			abs( ( val(2) - valPrev(2) ) ./ valPrev(2) ); ...
			abs( ( val(3) - valPrev(3) ) ./ valPrev(3) ) ...
		] );
		
		if( errRel > 1E-2 || isnan( errRel ) )
			valNext1 = AdaptiveSingIntVec( vr, v1, v12, v13, v14, level + 1, val1 );
			valNext2 = AdaptiveSingIntVec( vr, v2, v12, v23, v24, level + 1, val2 );
			valNext3 = AdaptiveSingIntVec( vr, v3, v13, v23, v34, level + 1, val3 );
			valNext4 = AdaptiveSingIntVec( vr, v4, v14, v24, v34, level + 1, val4 );
			valNext5 = AdaptiveSingIntVec( vr, v12, v13, v14, v23, level + 1, val5 );
			valNext6 = AdaptiveSingIntVec( vr, v12, v14, v23, v24, level + 1, val6 );
			valNext7 = AdaptiveSingIntVec( vr, v13, v14, v23, v34, level + 1, val7 );
			valNext8 = AdaptiveSingIntVec( vr, v14, v23, v24, v34, level + 1, val8 );
			result = valNext1 + valNext2 + valNext3 + valNext4 + valNext5 + valNext6 + valNext7 + valNext8;
		else
			result = val;
		end
		
	end
	
	
end

