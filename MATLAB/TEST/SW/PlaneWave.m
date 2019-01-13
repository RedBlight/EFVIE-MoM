classdef PlaneWave
	
	properties
		
		waveNum_;
		
		dir_;
		
		por_;
		
		polU_;
		ampU_;
		phsU_;
		
		polR_;
		ampR_;
		phsR_;
		
	end
	
	methods
		function obj = PlaneWave( ...
			waveNum, ...
			dirVec, ...
			polVec, ...
			porVec, ...
			ampU, ampR, ...
			phsU, phsR ...
		)
			obj.waveNum_ = waveNum;
			obj.dir_ = dirVec;
			obj.por_ = porVec;
			
			obj.polU_ = polVec;
			obj.polR_ = M.VecUnit( M.VecCross( obj.dir_, obj.polU_ ) );
			
			obj.ampU_ = ampU;
			obj.ampR_ = ampR;
			obj.phsU_ = phsU;
			obj.phsR_ = phsR;
			
		end
		
		function E = FieldE( obj, r )
			
			rp = M.ProjPlane( r, obj.dir_, obj.por_ );
			R = M.VecDot( r - rp, obj.dir_ );
			lamR = obj.waveNum_ * R;
			
			EU = exp( 1i * ( obj.phsU_ - lamR ) ) .* ( obj.ampU_ .* obj.polU_ );
			ER = exp( 1i * ( obj.phsR_ - lamR ) ) .* ( obj.ampR_ .* obj.polR_ );
			
			E = EU + ER;
			
		end
	end
end

