function result = SingIntVol( r_, v1_, v2_, v3_, v4_ )
	
	result = SingIntVol_Face( r_, v1_, v2_, v3_, v4_ );
	result = result + SingIntVol_Face( r_, v2_, v3_, v4_, v1_ );
	result = result + SingIntVol_Face( r_, v3_, v4_, v1_, v2_ );
	result = result + SingIntVol_Face( r_, v4_, v1_, v2_, v3_ );
	result = result / 2;
	
end

