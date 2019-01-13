function result = AdaptiveSingIntSur( vr, v1, v2, v3, level, valPrev )

	integrand = @( vr, vi ) 1 / M.VecLength( vi - vr );
	
	if( level > 8 )
		if( isinf( valPrev ) )
			result = 0;
		else
			result = valPrev;
		end
	else
		v12 = ( v1 + v2 ) ./ 2;
		v23 = ( v2 + v3 ) ./ 2;
		v31 = ( v3 + v1 ) ./ 2;
		
		val1 = M.TriArea( v1, v12, v31 ) * integrand( vr, M.TriMid( v1, v12, v31 ) );
		val2 = M.TriArea( v2, v12, v23 ) * integrand( vr, M.TriMid( v2, v12, v23 ) );
		val3 = M.TriArea( v3, v23, v31 ) * integrand( vr, M.TriMid( v3, v23, v31 ) );
		val4 = M.TriArea( v12, v23, v31 ) * integrand( vr, M.TriMid( v12, v23, v31 ) );
		
		val = val1 + val2 + val3 + val4;
		
		errRel = abs( ( val - valPrev ) / valPrev );
		if( errRel > 1E-2 || isnan( errRel ) )
			valNext1 = AdaptiveSingIntSur( vr, v1, v12, v31, level + 1, val1 );
			valNext2 = AdaptiveSingIntSur( vr, v2, v12, v23, level + 1, val2 );
			valNext3 = AdaptiveSingIntSur( vr, v3, v23, v31, level + 1, val3 );
			valNext4 = AdaptiveSingIntSur( vr, v12, v23, v31, level + 1, val4 );
			result = valNext1 + valNext2 + valNext3 + valNext4;
		else
			result = val;
		end
		
	end
	
	
end

