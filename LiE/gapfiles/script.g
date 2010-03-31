GenerateFunction := function( l, n )
    local   cmd,  cmd2,  cmd3,  f,  i,  j,  m;
    
    cmd := "#############################################################################";
    Append( cmd, "\n##\n" );
    cmd2 := "#F  ";
    Append( cmd2, "Lie" );
    Append( cmd2, n );
    cmd3 := "call the LiE function \"";
    Append( cmd3, n );
    Append( cmd3, "\"" );
    Append( cmd2, " " );
    if Length(cmd2) mod 2 = 0  then
        Append( cmd2, " " );
    fi;
    while Length(cmd3)+Length(cmd2) < 76  do
        Append( cmd2, ". " );
    od;
    if Length(cmd3)+Length(cmd2) = 76  then
        Append( cmd2, " " );
    fi;
    Append( cmd, cmd2 );
    Append( cmd, cmd3 );
    Append( cmd, "\n##\n" );
    f := Filtered( l, x -> x[1] = n );
    cmd := Concatenation( cmd, "Lie" );
    Append( cmd, n );
    Append( cmd, " := function( arg )\n    local   res;\n\n" );
    for i  in [ 1 .. Length(f) ]  do
        m := [ 1 .. Length(f[i])-2 ];
        if f[i][Length(f[i])] = GRPDFT  then
            f[i] := Copy(f[i]);
            f[i]{[4..Length(f[i])]} := f[i]{[3..Length(f[i])-1]};
            f[i][3] := GRPDFT;
            m := Concatenation( [2..Length(f[i])-2], [1] );
        fi;
        if i = 1  then
            Append( cmd, "    if " );
        else
            Append( cmd, "    elif " );
        fi;
        cmd := Concatenation( cmd, " Length(arg) = ", String(Length(f[i])-2) );
        for j  in [ 3  .. Length(f[i]) ]  do
            Append( cmd, "\n        and " );
            if f[i][j] = MATRIX  then
                cmd := Concatenation( cmd, "IsMatrix(arg[",String(j-2),"])" );
            elif f[i][j] = GRPDFT  then
                cmd := Concatenation( cmd, "IsLieGroup(arg[",String(j-2),"])" );
            elif f[i][j] = INTEGER  then
                cmd := Concatenation( cmd, "IsInt(arg[",String(j-2),"])" );
            elif f[i][j] = VECTOR  then
                cmd := Concatenation( cmd, "IsVector(arg[",String(j-2),"])" );
            elif f[i][j] = POLY  then
                cmd := Concatenation( cmd, "IsLiePolynomial(arg[",String(j-2),"])" );
            elif f[i][j] = GROUP  then
                cmd := Concatenation( cmd, "IsLieGroup(arg[",String(j-2),"])" );
            fi;
        od;
        cmd := Concatenation( cmd, "\n    then\n" );
        cmd := Concatenation( cmd, "        # return type: " );
        if f[i][2] = MATRIX  then
            cmd := Concatenation( cmd, "MATRIX\n" );
        elif f[i][2] = VECTOR  then
            cmd := Concatenation( cmd, "VECTOR\n" );
        elif  f[i][2] = POLY  then
            cmd := Concatenation( cmd, "POLY\n" );
        elif  f[i][2] = INTEGER  then
            cmd := Concatenation( cmd, "INTEGER\n" );
        elif  f[i][2] = GROUP  then
            cmd := Concatenation( cmd, "GROUP\n" );
        fi;
        cmd := Concatenation( cmd, "        res := CallLie(\"", n, "\"" );
        for j   in [ 3 .. Length(f[i]) ]  do
            cmd := Concatenation( cmd, ", " );
            cmd := Concatenation( cmd, "arg[", String(m[j-2]), "]" );
        od;
        cmd := Concatenation( cmd, ");\n" );
    od;
    cmd := Concatenation( cmd, "    else\n        Error( \"" );
    cmd := Concatenation( cmd, "LiE has no function \\\"", n, "\\\"",
                          " for these types" );
    cmd := Concatenation( cmd, "\" );\n    fi;\n    return res;" );
    cmd := Concatenation( cmd, "\n\nend;\n\n\n" );
    return cmd;
    
end;

