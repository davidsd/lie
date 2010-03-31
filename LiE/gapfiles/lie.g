#############################################################################
##
#F  LieAdams . . . . . . . . . . . . . . . . .  call the LiE function "Adams"
##
LieAdams := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("Adams", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsLiePoly(arg[3])
    then
        # return type: POLY
        res := CallLie("Adams", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"Adams\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieBruhat_desc . . . . . . . . . . .  call the LiE function "Bruhat_desc"
##
LieBruhat_desc := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("Bruhat_desc", arg[2], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: MATRIX
        res := CallLie("Bruhat_desc", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"Bruhat_desc\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieBruhat_leq  . . . . . . . . . . . . call the LiE function "Bruhat_leq"
##
LieBruhat_leq := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: INTEGER
        res := CallLie("Bruhat_leq", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"Bruhat_leq\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieCartan  . . . . . . . . . . . . . . . . call the LiE function "Cartan"
##
LieCartan := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: INTEGER
        res := CallLie("Cartan", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: MATRIX
        res := CallLie("Cartan", arg[1]);
    else
        Error( "LiE has no function \"Cartan\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieCartan_type . . . . . . . . . . .  call the LiE function "Cartan_type"
##
LieCartan_type := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: GROUP
        res := CallLie("Cartan_type", arg[2], arg[1]);
    else
        Error( "LiE has no function \"Cartan_type\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieDemazure  . . . . . . . . . . . . . . call the LiE function "Demazure"
##
LieDemazure := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("Demazure", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("Demazure", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("Demazure", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: POLY
        res := CallLie("Demazure", arg[2], arg[1]);
    else
        Error( "LiE has no function \"Demazure\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieKL_poly . . . . . . . . . . . . . . .  call the LiE function "KL_poly"
##
LieKL_poly := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("KL_poly", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"KL_poly\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieLR_tensor . . . . . . . . . . . . .  call the LiE function "LR_tensor"
##
LieLR_tensor := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsVector(arg[1])
        and IsVector(arg[2])
    then
        # return type: POLY
        res := CallLie("LR_tensor", arg[1], arg[2]);
    elif  Length(arg) = 2
        and IsLiePoly(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("LR_tensor", arg[1], arg[2]);
    else
        Error( "LiE has no function \"LR_tensor\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieRS  . . . . . . . . . . . . . . . . . . . . call the LiE function "RS"
##
LieRS := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: MATRIX
        res := CallLie("RS", arg[1]);
    elif  Length(arg) = 2
        and IsVector(arg[1])
        and IsVector(arg[2])
    then
        # return type: VECTOR
        res := CallLie("RS", arg[1], arg[2]);
    else
        Error( "LiE has no function \"RS\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieR_poly  . . . . . . . . . . . . . . . . call the LiE function "R_poly"
##
LieR_poly := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("R_poly", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"R_poly\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_action  . . . . . . . . . . . . . . call the LiE function "W_action"
##
LieW_action := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: VECTOR
        res := CallLie("W_action", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("W_action", arg[2], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
        and IsVector(arg[3])
    then
        # return type: MATRIX
        res := CallLie("W_action", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("W_action", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"W_action\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_orbit . . . . . . . . . . . . . . .  call the LiE function "W_orbit"
##
LieW_orbit := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("W_orbit", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("W_orbit", arg[2], arg[1]);
    else
        Error( "LiE has no function \"W_orbit\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_orbit_size  . . . . . . . . . . call the LiE function "W_orbit_size"
##
LieW_orbit_size := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: INTEGER
        res := CallLie("W_orbit_size", arg[2], arg[1]);
    else
        Error( "LiE has no function \"W_orbit_size\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_order . . . . . . . . . . . . . . .  call the LiE function "W_order"
##
LieW_order := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: INTEGER
        res := CallLie("W_order", arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: INTEGER
        res := CallLie("W_order", arg[2], arg[1]);
    else
        Error( "LiE has no function \"W_order\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_rt_action . . . . . . . . . . .  call the LiE function "W_rt_action"
##
LieW_rt_action := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: VECTOR
        res := CallLie("W_rt_action", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("W_rt_action", arg[2], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
        and IsVector(arg[3])
    then
        # return type: MATRIX
        res := CallLie("W_rt_action", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"W_rt_action\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_rt_orbit  . . . . . . . . . . . . call the LiE function "W_rt_orbit"
##
LieW_rt_orbit := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("W_rt_orbit", arg[2], arg[1]);
    else
        Error( "LiE has no function \"W_rt_orbit\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  LieW_word  . . . . . . . . . . . . . . . . call the LiE function "W_word"
##
LieW_word := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: VECTOR
        res := CallLie("W_word", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: VECTOR
        res := CallLie("W_word", arg[2], arg[1]);
    else
        Error( "LiE has no function \"W_word\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieadjoint . . . . . . . . . . . . . . .  call the LiE function "adjoint"
##
Lieadjoint := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: POLY
        res := CallLie("adjoint", arg[1]);
    else
        Error( "LiE has no function \"adjoint\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liealt_W_sum . . . . . . . . . . . . .  call the LiE function "alt_W_sum"
##
Liealt_W_sum := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("alt_W_sum", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: POLY
        res := CallLie("alt_W_sum", arg[2], arg[1]);
    else
        Error( "LiE has no function \"alt_W_sum\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liealt_dom . . . . . . . . . . . . . . .  call the LiE function "alt_dom"
##
Liealt_dom := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("alt_dom", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: POLY
        res := CallLie("alt_dom", arg[2], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("alt_dom", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("alt_dom", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"alt_dom\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liealt_tensor  . . . . . . . . . . . . call the LiE function "alt_tensor"
##
Liealt_tensor := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("alt_tensor", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsLiePoly(arg[3])
    then
        # return type: POLY
        res := CallLie("alt_tensor", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"alt_tensor\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liebranch  . . . . . . . . . . . . . . . . call the LiE function "branch"
##
Liebranch := function( arg )
    local   res;

    if  Length(arg) = 4
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsLieGroup(arg[3])
        and IsMatrix(arg[4])
    then
        # return type: POLY
        res := CallLie("branch", arg[2], arg[3], arg[4], arg[1]);
    elif  Length(arg) = 4
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsLieGroup(arg[3])
        and IsMatrix(arg[4])
    then
        # return type: POLY
        res := CallLie("branch", arg[2], arg[3], arg[4], arg[1]);
    else
        Error( "LiE has no function \"branch\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liecanonical . . . . . . . . . . . . .  call the LiE function "canonical"
##
Liecanonical := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: VECTOR
        res := CallLie("canonical", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("canonical", arg[2], arg[1]);
    else
        Error( "LiE has no function \"canonical\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liecent_roots  . . . . . . . . . . . . call the LiE function "cent_roots"
##
Liecent_roots := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("cent_roots", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("cent_roots", arg[2], arg[1]);
    else
        Error( "LiE has no function \"cent_roots\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liecenter  . . . . . . . . . . . . . . . . call the LiE function "center"
##
Liecenter := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: MATRIX
        res := CallLie("center", arg[1]);
    else
        Error( "LiE has no function \"center\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liecentr_type  . . . . . . . . . . . . call the LiE function "centr_type"
##
Liecentr_type := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: GROUP
        res := CallLie("centr_type", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: GROUP
        res := CallLie("centr_type", arg[2], arg[1]);
    else
        Error( "LiE has no function \"centr_type\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieclass_ord . . . . . . . . . . . . .  call the LiE function "class_ord"
##
Lieclass_ord := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: INTEGER
        res := CallLie("class_ord", arg[1]);
    else
        Error( "LiE has no function \"class_ord\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieclosure . . . . . . . . . . . . . . .  call the LiE function "closure"
##
Lieclosure := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("closure", arg[2], arg[1]);
    else
        Error( "LiE has no function \"closure\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liecollect . . . . . . . . . . . . . . .  call the LiE function "collect"
##
Liecollect := function( arg )
    local   res;

    if  Length(arg) = 4
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsLieGroup(arg[3])
        and IsMatrix(arg[4])
    then
        # return type: POLY
        res := CallLie("collect", arg[2], arg[3], arg[4], arg[1]);
    elif  Length(arg) = 5
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsLieGroup(arg[3])
        and IsMatrix(arg[4])
        and IsInt(arg[5])
    then
        # return type: POLY
        res := CallLie("collect", arg[2], arg[3], arg[4], arg[5], arg[1]);
    else
        Error( "LiE has no function \"collect\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liecontragr  . . . . . . . . . . . . . . call the LiE function "contragr"
##
Liecontragr := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: VECTOR
        res := CallLie("contragr", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("contragr", arg[2], arg[1]);
    else
        Error( "LiE has no function \"contragr\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liedecomp  . . . . . . . . . . . . . . . . call the LiE function "decomp"
##
Liedecomp := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("decomp", arg[2], arg[1]);
    else
        Error( "LiE has no function \"decomp\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liedet_Cartan  . . . . . . . . . . . . call the LiE function "det_Cartan"
##
Liedet_Cartan := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: INTEGER
        res := CallLie("det_Cartan", arg[1]);
    else
        Error( "LiE has no function \"det_Cartan\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liedim . . . . . . . . . . . . . . . . . . .  call the LiE function "dim"
##
Liedim := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: INTEGER
        res := CallLie("dim", arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: INTEGER
        res := CallLie("dim", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: INTEGER
        res := CallLie("dim", arg[2], arg[1]);
    else
        Error( "LiE has no function \"dim\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liedom_char  . . . . . . . . . . . . . . call the LiE function "dom_char"
##
Liedom_char := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: INTEGER
        res := CallLie("dom_char", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsVector(arg[3])
    then
        # return type: INTEGER
        res := CallLie("dom_char", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: POLY
        res := CallLie("dom_char", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("dom_char", arg[2], arg[1]);
    else
        Error( "LiE has no function \"dom_char\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liedom_weights . . . . . . . . . . .  call the LiE function "dom_weights"
##
Liedom_weights := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("dom_weights", arg[2], arg[1]);
    else
        Error( "LiE has no function \"dom_weights\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liedominant  . . . . . . . . . . . . . . call the LiE function "dominant"
##
Liedominant := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: VECTOR
        res := CallLie("dominant", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("dominant", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("dominant", arg[2], arg[1]);
    else
        Error( "LiE has no function \"dominant\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieexponents . . . . . . . . . . . . .  call the LiE function "exponents"
##
Lieexponents := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: VECTOR
        res := CallLie("exponents", arg[1]);
    else
        Error( "LiE has no function \"exponents\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liefilter_dom  . . . . . . . . . . . . call the LiE function "filter_dom"
##
Liefilter_dom := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("filter_dom", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("filter_dom", arg[2], arg[1]);
    else
        Error( "LiE has no function \"filter_dom\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liefrom_part . . . . . . . . . . . . .  call the LiE function "from_part"
##
Liefrom_part := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("from_part", arg[1]);
    elif  Length(arg) = 1
        and IsMatrix(arg[1])
    then
        # return type: MATRIX
        res := CallLie("from_part", arg[1]);
    elif  Length(arg) = 1
        and IsLiePoly(arg[1])
    then
        # return type: POLY
        res := CallLie("from_part", arg[1]);
    else
        Error( "LiE has no function \"from_part\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liefundam  . . . . . . . . . . . . . . . . call the LiE function "fundam"
##
Liefundam := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("fundam", arg[2], arg[1]);
    else
        Error( "LiE has no function \"fundam\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liehigh_root . . . . . . . . . . . . .  call the LiE function "high_root"
##
Liehigh_root := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: VECTOR
        res := CallLie("high_root", arg[1]);
    else
        Error( "LiE has no function \"high_root\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liei_Cartan  . . . . . . . . . . . . . . call the LiE function "i_Cartan"
##
Liei_Cartan := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: MATRIX
        res := CallLie("i_Cartan", arg[1]);
    else
        Error( "LiE has no function \"i_Cartan\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieinprod  . . . . . . . . . . . . . . . . call the LiE function "inprod"
##
Lieinprod := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: INTEGER
        res := CallLie("inprod", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"inprod\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liel_reduce  . . . . . . . . . . . . . . call the LiE function "l_reduce"
##
Liel_reduce := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: VECTOR
        res := CallLie("l_reduce", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"l_reduce\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lielength  . . . . . . . . . . . . . . . . call the LiE function "length"
##
Lielength := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: INTEGER
        res := CallLie("length", arg[2], arg[1]);
    else
        Error( "LiE has no function \"length\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lielong_word . . . . . . . . . . . . .  call the LiE function "long_word"
##
Lielong_word := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: VECTOR
        res := CallLie("long_word", arg[1]);
    else
        Error( "LiE has no function \"long_word\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lielr_reduce . . . . . . . . . . . . .  call the LiE function "lr_reduce"
##
Lielr_reduce := function( arg )
    local   res;

    if  Length(arg) = 4
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
        and IsVector(arg[4])
    then
        # return type: VECTOR
        res := CallLie("lr_reduce", arg[2], arg[3], arg[4], arg[1]);
    else
        Error( "LiE has no function \"lr_reduce\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lien_pos_roots . . . . . . . . . . .  call the LiE function "n_pos_roots"
##
Lien_pos_roots := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: INTEGER
        res := CallLie("n_pos_roots", arg[1]);
    else
        Error( "LiE has no function \"n_pos_roots\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lien_tabl  . . . . . . . . . . . . . . . . call the LiE function "n_tabl"
##
Lien_tabl := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: INTEGER
        res := CallLie("n_tabl", arg[1]);
    else
        Error( "LiE has no function \"n_tabl\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lienext_part . . . . . . . . . . . . .  call the LiE function "next_part"
##
Lienext_part := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("next_part", arg[1]);
    else
        Error( "LiE has no function \"next_part\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lienext_perm . . . . . . . . . . . . .  call the LiE function "next_perm"
##
Lienext_perm := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("next_perm", arg[1]);
    else
        Error( "LiE has no function \"next_perm\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lienext_tabl . . . . . . . . . . . . .  call the LiE function "next_tabl"
##
Lienext_tabl := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("next_tabl", arg[1]);
    else
        Error( "LiE has no function \"next_tabl\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lienorm  . . . . . . . . . . . . . . . . . . call the LiE function "norm"
##
Lienorm := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: INTEGER
        res := CallLie("norm", arg[2], arg[1]);
    else
        Error( "LiE has no function \"norm\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieorbit . . . . . . . . . . . . . . . . .  call the LiE function "orbit"
##
Lieorbit := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsInt(arg[1])
        and IsVector(arg[2])
        and IsMatrix(arg[3])
    then
        # return type: MATRIX
        res := CallLie("orbit", arg[1], arg[2], arg[3]);
    elif  Length(arg) = 2
        and IsVector(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("orbit", arg[1], arg[2]);
    else
        Error( "LiE has no function \"orbit\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liep_tensor  . . . . . . . . . . . . . . call the LiE function "p_tensor"
##
Liep_tensor := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("p_tensor", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsLiePoly(arg[3])
    then
        # return type: POLY
        res := CallLie("p_tensor", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"p_tensor\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liepartitions  . . . . . . . . . . . . call the LiE function "partitions"
##
Liepartitions := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsInt(arg[1])
    then
        # return type: MATRIX
        res := CallLie("partitions", arg[1]);
    else
        Error( "LiE has no function \"partitions\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieplethysm  . . . . . . . . . . . . . . call the LiE function "plethysm"
##
Lieplethysm := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("plethysm", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsLiePoly(arg[3])
    then
        # return type: POLY
        res := CallLie("plethysm", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"plethysm\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liepos_roots . . . . . . . . . . . . .  call the LiE function "pos_roots"
##
Liepos_roots := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsLieGroup(arg[1])
    then
        # return type: MATRIX
        res := CallLie("pos_roots", arg[1]);
    else
        Error( "LiE has no function \"pos_roots\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lier_reduce  . . . . . . . . . . . . . . call the LiE function "r_reduce"
##
Lier_reduce := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: VECTOR
        res := CallLie("r_reduce", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"r_reduce\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liereduce  . . . . . . . . . . . . . . . . call the LiE function "reduce"
##
Liereduce := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: VECTOR
        res := CallLie("reduce", arg[2], arg[1]);
    else
        Error( "LiE has no function \"reduce\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liereflection  . . . . . . . . . . . . call the LiE function "reflection"
##
Liereflection := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: MATRIX
        res := CallLie("reflection", arg[2], arg[1]);
    else
        Error( "LiE has no function \"reflection\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieres_mat . . . . . . . . . . . . . . .  call the LiE function "res_mat"
##
Lieres_mat := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsMatrix(arg[2])
    then
        # return type: MATRIX
        res := CallLie("res_mat", arg[2], arg[1]);
    else
        Error( "LiE has no function \"res_mat\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieshape . . . . . . . . . . . . . . . . .  call the LiE function "shape"
##
Lieshape := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("shape", arg[1]);
    else
        Error( "LiE has no function \"shape\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liesign_part . . . . . . . . . . . . .  call the LiE function "sign_part"
##
Liesign_part := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: INTEGER
        res := CallLie("sign_part", arg[1]);
    else
        Error( "LiE has no function \"sign_part\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liespectrum  . . . . . . . . . . . . . . call the LiE function "spectrum"
##
Liespectrum := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("spectrum", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("spectrum", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"spectrum\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liesym_char  . . . . . . . . . . . . . . call the LiE function "sym_char"
##
Liesym_char := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsVector(arg[1])
        and IsVector(arg[2])
    then
        # return type: INTEGER
        res := CallLie("sym_char", arg[1], arg[2]);
    elif  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: MATRIX
        res := CallLie("sym_char", arg[1]);
    else
        Error( "LiE has no function \"sym_char\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liesym_orbit . . . . . . . . . . . . .  call the LiE function "sym_orbit"
##
Liesym_orbit := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: MATRIX
        res := CallLie("sym_orbit", arg[1]);
    else
        Error( "LiE has no function \"sym_orbit\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liesym_tensor  . . . . . . . . . . . . call the LiE function "sym_tensor"
##
Liesym_tensor := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("sym_tensor", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsInt(arg[2])
        and IsLiePoly(arg[3])
    then
        # return type: POLY
        res := CallLie("sym_tensor", arg[2], arg[3], arg[1]);
    else
        Error( "LiE has no function \"sym_tensor\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lietableaux  . . . . . . . . . . . . . . call the LiE function "tableaux"
##
Lietableaux := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: MATRIX
        res := CallLie("tableaux", arg[1]);
    else
        Error( "LiE has no function \"tableaux\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lietensor  . . . . . . . . . . . . . . . . call the LiE function "tensor"
##
Lietensor := function( arg )
    local   res;

    if  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
    then
        # return type: POLY
        res := CallLie("tensor", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 3
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsLiePoly(arg[3])
    then
        # return type: POLY
        res := CallLie("tensor", arg[2], arg[3], arg[1]);
    elif  Length(arg) = 4
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
        and IsVector(arg[3])
        and IsVector(arg[4])
    then
        # return type: INTEGER
        res := CallLie("tensor", arg[2], arg[3], arg[4], arg[1]);
    elif  Length(arg) = 4
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
        and IsLiePoly(arg[3])
        and IsVector(arg[4])
    then
        # return type: INTEGER
        res := CallLie("tensor", arg[2], arg[3], arg[4], arg[1]);
    else
        Error( "LiE has no function \"tensor\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lieto_part . . . . . . . . . . . . . . .  call the LiE function "to_part"
##
Lieto_part := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("to_part", arg[1]);
    elif  Length(arg) = 1
        and IsMatrix(arg[1])
    then
        # return type: MATRIX
        res := CallLie("to_part", arg[1]);
    elif  Length(arg) = 1
        and IsLiePoly(arg[1])
    then
        # return type: POLY
        res := CallLie("to_part", arg[1]);
    else
        Error( "LiE has no function \"to_part\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Lietrans_part  . . . . . . . . . . . . call the LiE function "trans_part"
##
Lietrans_part := function( arg )
    local   res;

    if  Length(arg) = 1
        and IsVector(arg[1])
    then
        # return type: VECTOR
        res := CallLie("trans_part", arg[1]);
    else
        Error( "LiE has no function \"trans_part\" for these types" );
    fi;
    return res;

end;


#############################################################################
##
#F  Liev_decomp  . . . . . . . . . . . . . . call the LiE function "v_decomp"
##
Liev_decomp := function( arg )
    local   res;

    if  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsLiePoly(arg[2])
    then
        # return type: POLY
        res := CallLie("v_decomp", arg[2], arg[1]);
    elif  Length(arg) = 2
        and IsLieGroup(arg[1])
        and IsVector(arg[2])
    then
        # return type: POLY
        res := CallLie("v_decomp", arg[2], arg[1]);
    else
        Error( "LiE has no function \"v_decomp\" for these types" );
    fi;
    return res;

end;


