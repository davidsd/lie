

bigint* fac(index n);

bigint* n_tableaux(entry* lambda, index l);

bigint* Classord(entry* kappa, index l);

index check_part(entry* lambda, index l);

vector* check_tabl(vector* v);

boolean Nextperm(entry* w, index n);

boolean Nextpart(entry* lambda, index l);

boolean Nexttableau(entry* t, index n);

matrix* Permutations(entry* v,index n);

matrix* Partitions(index n);

matrix* Tableaux(entry* lambda, index l);

vector* Trans_part(entry* lambda, index l);

entry Sign_part(entry* lambda, index l);

void Robinson_Schensted (entry* P, entry* Q, index n, entry* sigma);

void Schensted_Robinson (entry* sigma, index n, entry* P, entry* Q);

poly* MN_char(entry* lambda, index l);

bigint* MN_char_val(entry* lambda, entry* mu, index l, index m);


