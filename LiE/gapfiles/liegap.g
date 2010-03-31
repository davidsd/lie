LIE := 0;

EXEC := function( arg )

	Exec( Concatenation( List( arg, String ) ) );
end;

StringLiePolynomial := function( l )
    local   cmd,  i;
    
    cmd := "";
    for i  in [ 1, 3 .. Length(l)-1 ]  do
        if 1 < i  then
            Append( cmd, "+\n" );
        fi;
        Append( cmd,  StringInt(l[i]) );
        Append( cmd, "X" );
        Append( cmd, String(l[i+1]) );
    od;
    return cmd;
end;

IsMatrix := IsMat;

IsLieGroup := IsString;

IsLiePoly := function(obj)
    return IsList(obj) and 1 < Length(obj)
           and IsInt(obj[1]) and IsList(obj[2]);
end;

CallLie := function( arg )
    local   intmp,  outtmp,  cmd,  i;
    
    intmp := TmpName();  EXEC( "touch ", intmp );
    outtmp := TmpName(); EXEC( "touch ", outtmp );
    cmd := "LIE = ";
    Append( cmd, arg[1] );
    Append( cmd, "( " );
    for i  in [ 2  .. Length(arg) ]  do
        if 2 < i  then
            Append( cmd, ", " );
        fi;
        if IsLiePoly(arg[i])  then
            Append( cmd, StringLiePolynomial(arg[i]) );
        else
            Append( cmd, String(arg[i]) );
        fi;
    od;
    Append( cmd, " )\n" );
    Append( cmd, "? LIE > " );
    Append( cmd, outtmp );
    Append( cmd, "\n" );
    PrintTo( intmp, cmd );
    Unbind(LIE);
    EXEC( "cd LiE; ./Liegap.exe > /dev/null < ", intmp );
    Read(outtmp);
    return LIE;
end;
