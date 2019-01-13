function [ epsRel ] = GenerateEpsRel_Cut( tetraMidPoint, epsr1, rlim, epsr2 )
	
	[ ~, ~, r ] = cart2sph( tetraMidPoint( 1, : ), tetraMidPoint( 2, : ), tetraMidPoint( 3, : ) );
	rFlag = r <= rlim;
	epsRel = double( rFlag ) .* epsr1 + double( ~rFlag ) .* epsr2;
	
end