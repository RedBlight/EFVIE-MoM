function [ tetraCount, emPropData ] = LoadEmProps( geomName )
    
    p = load( "..\Misc\pathList.mat" );
	propPath = p.dataFolder + geomName + p.propExt;
    
    fileId = fopen( propPath );
    
    tetraCount = fread( fileId, 1, 'uint64' );
    
    emPropData = fread( fileId, tetraCount * 2, 'double' );
    emPropData = transpose( reshape( emPropData, [ 2, faceCount ] ) );
    
    fclose( fileId );
	
end

