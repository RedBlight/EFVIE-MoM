function rcsData = ReadRcsResult( fileName )
    
    fileID = fopen( "Data/" + fileName + ".rcsresult" );

    obsCount = fread( fileID, 1, 'uint64' );
    rcsData = fread( fileID, obsCount, 'double' );

    fclose( fileID );

end

