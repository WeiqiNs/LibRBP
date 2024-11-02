#include "field.hpp"

Field::Field(const Fp& order){
    prime = order;
}

Field::Field(const int& order){
    bn_set_dig(prime.value, order);
}

bool Field::cmp(const Fp& x, const Fp& y){
    return bn_cmp(x.value, y.value) == RLC_EQ;
}

bool Field::cmp(const Fp& x, const int& y){
    return bn_cmp_dig(x.value, y) == RLC_EQ;
}

Fp Field::one(){
    Fp r;
    bn_set_dig(r.value, 1);
    return r;
}

Fp Field::zero(){
    Fp r;
    bn_set_dig(r.value, 0);
    return r;
}

void Field::mod(Fp& x) const{
    bn_mod(x.value, x.value, prime.value);
}

Fp Field::from_int(const int& x) const{
    Fp r;

    // Check if the input integer is negative.
    if (x >= 0){
        bn_set_dig(r.value, x);
        mod(r);
    } else{
        bn_set_dig(r.value, -x);
        mod(r);
        r = neg(r);
    }

    return r;
}

Fp Field::rand() const{
    Fp r;
    bn_rand_mod(r.value, prime.value);
    return r;
}

Fp Field::add(const Fp& x, const Fp& y) const{
    Fp r;
    bn_add(r.value, x.value, y.value);
    mod(r);
    return r;
}

Fp Field::sub(const Fp& x, const Fp& y) const{
    Fp r;
    bn_sub(r.value, x.value, y.value);
    mod(r);
    return r;
}

Fp Field::mul(const Fp& x, const Fp& y) const{
    Fp r;
    bn_mul(r.value, x.value, y.value);
    mod(r);
    return r;
}

Fp Field::exp(const Fp& x, const Fp& y) const{
    Fp r;
    bn_mxp(r.value, x.value, y.value, prime.value);
    return r;
}

Fp Field::neg(const Fp& x) const{
    Fp r;
    bn_neg(r.value, x.value);
    mod(r);
    return r;
}

Fp Field::inv(const Fp& x) const{
    Fp r;
    bn_mod_inv(r.value, x.value, prime.value);
    return r;
}

void Field::mod(FpVec& x) const{
    for (auto i : x) mod(i);
}

FpVec Field::vec_join(const FpVec& x, const FpVec& y){
    FpVec r;
    for (const auto& i : x) r.push_back(i);
    for (const auto& i : y) r.push_back(i);
    return r;
}

FpVec Field::from_int(const IntVec& x) const{
    FpVec r;

    for (int i : x) r.push_back(from_int(i));

    return r;
}

FpVec Field::rand_vec(const int& size) const{
    FpVec r;

    for (int i = 0; i < size; ++i) r.push_back(rand());

    return r;
}

FpVec Field::vec_add(const FpVec& x, const FpVec& y) const{
    if (x.size() != y.size())
        throw std::runtime_error("The input vectors are of different lengths.");

    FpVec r;

    for (int i = 0; i < x.size(); ++i) r.push_back(add(x[i], y[i]));

    return r;
}

FpVec Field::vec_sub(const FpVec& x, const FpVec& y) const{
    if (x.size() != y.size())
        throw std::runtime_error("The input vectors are of different lengths.");

    FpVec r;

    for (int i = 0; i < x.size(); ++i) r.push_back(sub(x[i], y[i]));

    return r;
}

FpVec Field::vec_mul(const FpVec& x, const FpVec& y) const{
    if (x.size() != y.size())
        throw std::runtime_error("The input vectors are of different lengths.");

    FpVec r;

    for (int i = 0; i < x.size(); ++i) r.push_back(mul(x[i], y[i]));

    return r;
}

FpVec Field::vec_mul(const FpVec& x, const Fp& y) const{
    FpVec r;

    for (const auto& i : x) r.push_back(mul(i, y));

    return r;
}

FpVec Field::vec_inv(const FpVec& x) const{
    FpVec r;

    for (const auto& i : x) r.push_back(inv(i));

    return r;
}

Fp Field::vec_ip(const FpVec& x, const FpVec& y) const{
    if (x.size() != y.size())
        throw std::runtime_error("The input vectors are of different lengths.");

    Fp ip = zero();

    FpVec r = vec_mul(x, y);

    for (const auto& i : r) ip = add(i, ip);

    return ip;
}

FpVec Field::poly_interpolate(const int& degree, const FpVec& roots) const{
    // Check if the input is legal.
    if (roots.size() > degree)
        throw std::runtime_error("Their are too many roots provided.");

    // When the input is just a zero, we output all zeros.
    if (roots.size() == 1 && cmp(roots[0], 0) ){
        const IntVec zero_vec(degree + 1, 0);
        return from_int(zero_vec);
    }

    // Declare a temp zp variable and the result zp vector.
    FpVec coeff(roots.size() + 1);

    // Initialize with leading coefficient 1 for x^0.
    coeff[0] = one();

    // Compute the coefficients.
    for (int i = 0; i < roots.size(); ++i){
        for (int j = i; j >= 0; --j){
            coeff[j + 1] = add(coeff[j + 1], neg(mul(coeff[j], roots[i])));
        }
    }

    // Invert the coefficients to return c + a0 + a1 + ...
    reverse(coeff.begin(), coeff.end());

    // Attach additional zeros if needed.
    if (degree > roots.size())
        coeff = vec_join(coeff, FpVec(degree - roots.size()));

    return coeff;
}

bool Field::mat_is_id(const FpMat& x){
    const auto size = x.size();

    for (int i = 0; i < size; i++){
        if (x[i].size() != size) return false;
        for (int j = 0; j < size; j++){
            if (i == j && !cmp(x[i][j], 1)) return false;
            if (i != j && !cmp(x[i][j], 0)) return false;
        }
    }

    return true;
}

FpMat Field::mat_id(const int& size){
    FpMat r(size, FpVec(size));

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            if (i == j) r[i][i] = one();
            // else r[i][j] = zero();
        }
    }

    return r;
}

FpMat Field::mat_transpose(const FpMat& x){
    const auto row = x.size();
    const auto col = x[0].size();

    FpMat r(col, FpVec(row));

    for (int i = 0; i < row; ++i) for (int j = 0; j < col; ++j)r[j][i] = x[i][j];

    return r;
}

FpMat Field::mat_join(const FpMat& x, const FpMat& y){
    // Check that inputs have to have equal length.
    if (x.size() != y.size())
        throw std::runtime_error("The input matrices are of different sizes.");

    FpMat r(x.size());

    for (int i = 0; i < x.size(); i++) r[i] = vec_join(x[i], y[i]);

    return r;
}

FpMat Field::from_int(const IntMat& x) const{
    FpMat r;

    for (const auto& i : x) r.push_back(from_int(i));

    return r;
}

FpMat Field::rand_mat(const int& row, const int& col) const{
    FpMat r;

    for (int i = 0; i < row; ++i) r.push_back(rand_vec(col));

    return r;
}

FpVec Field::mat_mul(const FpMat& x, const FpVec& y) const{
    // Check that inputs have to have equal length.
    if (x[0].size() != y.size())
        throw std::runtime_error("The input matrix col size is different from the input vector size.");

    FpVec r;

    for (const auto& i : x) r.push_back(vec_ip(i, y));

    return r;
}

FpVec Field::mat_mul(const FpVec& x, const FpMat& y) const{
    // Check that inputs have to have equal length.
    if (x.size() != y.size())
        throw std::runtime_error("The input vector size is different from the input matrix row size.");

    FpVec r(y[0].size());

    for (int i = 0; i < y[0].size(); ++i){
        for (int j = 0; j < y.size(); ++j){
            r[i] = add(r[i], mul(x[j], y[j][i]));
        }
    }

    return r;
}

FpMat Field::mat_mul(const FpMat& x, const FpMat& y) const{
    // Check that inputs have to have equal length.
    if (x[0].size() != y.size())
        throw std::runtime_error("The input matrix x's col size is different from y's row size.");

    const auto rowx = x.size();
    const auto colx = y.size();
    const auto coly = y[0].size();

    FpMat r(rowx, FpVec(coly));

    for (int i = 0; i < rowx; ++i){
        for (int j = 0; j < coly; ++j){
            for (int k = 0; k < colx; ++k){
                r[i][j] = add(r[i][j], mul(x[i][k], y[k][j]));
            }
        }
    }

    return r;
}

FpMat Field::mat_mul(const FpMat& x, const Fp& y) const{
    FpMat r;

    for (const auto& i : x) r.push_back(vec_mul(i, y));

    return r;
}

FpMat Field::mat_inv(const FpMat& x) const{
    // Check that input matrix is a square matrix.
    if (x.size() != x[0].size()){
        throw std::runtime_error("The input matrix is not a square matrix.");
    }

    // This cast should be safe as we won't be handling really large matrix.
    const int size = static_cast<int>(x.size());

    // Declare a value that will be reused.
    Fp temp_mul;

    // Generate the row echelon matrix.
    FpMat row_echelon = mat_join(x, mat_id(size));

    // Bottom left half to all zeros.
    for (int i = 0; i < size; i++){
        for (int j = i; j < size; j++){
            if (i == j && !cmp(row_echelon[i][j], 1)){
                temp_mul = inv(row_echelon[i][i]);
                for (int k = i; k < size * 2; k++)
                    row_echelon[j][k] = mul(row_echelon[j][k], temp_mul);
            }

            if (i == j && cmp(row_echelon[i][j], 0)) break;

            if (i != j){
                temp_mul = row_echelon[j][i];
                for (int k = i; k < size * 2; k++)
                    row_echelon[j][k] = sub(row_echelon[j][k], mul(temp_mul, row_echelon[i][k]));
            }
        }
    }

    // Top right half to all zeros.
    for (int i = size - 1; i > 0; i--){
        for (int j = i - 1; j >= 0; j--){
            temp_mul = row_echelon[j][i];
            for (int k = i; k < size * 2; k++)
                row_echelon[j][k] = sub(row_echelon[j][k], mul(temp_mul, row_echelon[i][k]));
        }
    }

    // Copy over the output.
    FpMat xi;
    for (const auto& i : row_echelon) xi.emplace_back(i.begin() + size, i.end());

    return xi;
}

FpMat Field::mat_inv_with_det(const FpMat& x, Fp& det) const{
    // Check that input matrix is a square matrix.
    if (x.size() != x[0].size()){
        throw std::runtime_error("The input matrix is not a square matrix.");
    }

    // This cast should be safe as we won't be handling really large matrix.
    const int size = static_cast<int>(x.size());

    // Declare a value that will be reused.
    Fp temp_mul;

    // Generate the row echelon matrix.
    FpMat row_echelon = mat_join(x, mat_id(size));

    // Set the starting determinant to 1.
    det = one();

    // Bottom left half to all zeros.
    for (int i = 0; i < size; i++){
        for (int j = i; j < size; j++){
            if (i == j && !cmp(row_echelon[i][j], 1)){
                det = mul(det, row_echelon[i][j]);
                temp_mul = inv(row_echelon[i][i]);
                for (int k = i; k < size * 2; k++)
                    row_echelon[j][k] = mul(row_echelon[j][k], temp_mul);
            }

            if (i == j && cmp(row_echelon[i][j], 0)) break;

            if (i != j){
                temp_mul = row_echelon[j][i];
                for (int k = i; k < size * 2; k++)
                    row_echelon[j][k] = sub(row_echelon[j][k], mul(temp_mul, row_echelon[i][k]));
            }
        }
    }

    // Top right half to all zeros.
    for (int i = size - 1; i > 0; i--){
        for (int j = i - 1; j >= 0; j--){
            temp_mul = row_echelon[j][i];
            for (int k = i; k < size * 2; k++)
                row_echelon[j][k] = sub(row_echelon[j][k], mul(temp_mul, row_echelon[i][k]));
        }
    }

    // Copy over the output.
    FpMat xi;
    for (const auto& i : row_echelon) xi.emplace_back(i.begin() + size, i.end());

    return xi;
}
