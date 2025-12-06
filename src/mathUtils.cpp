#include <cstdio>
#include <cstring>
#include <functional>
#include <utility>
#include <cmath>
#include <vector>
#include <numeric>
#include <set>

std::function<double(double)> polynomialEvaluator(std::vector<double>polinomio);
std::function<double(double)> negatefunction(std::function<double(double)> func);
double evaluatePolynomial(std::vector<double> polinomio, double t);
std::vector<double> multiplyPolinomials(std::vector<double> p1, std::vector<double> p2);
std::vector<double> polynomialDivision(std::vector<double> dividend, std::vector<double> divisor);
std::multiset<double> baskhara(std::vector<double> polinomio);

//(func - target) must have an odd number of roots between minimum and maximum (counting repeats)
double boundedBitSearch(std::function<double(double)> func, double target, double minimum, double maximum);
std::vector<double> deriva (std::vector<double> polinomio);
std::multiset<double> raizes(std::vector<double> polinomio);

std::multiset<double> raizes(std::vector<double> polinomio){

    long long degree;

    std::multiset<double> roots;

    for(degree = polinomio.size() - 1; polinomio[degree] == 0.0 && degree > 0; degree--);

    switch(degree){

        case 0ul:
            break;

    	case 1ul:
    	    roots.insert( - (polinomio[0] / polinomio[1]) );
    	    break;

    	case 2ul:
    	    roots = baskhara(polinomio);
    	    break;

    	default:

    	    if(degree % 2){

                double r1 = boundedBitSearch(polynomialEvaluator(polinomio), 0.0, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

                roots = raizes( polynomialDivision( polinomio, std::vector<double>{-r1, 1.0} ) );
                roots.insert(r1);

    	    }else{

                std::multiset<double> raizesDerivada = raizes(deriva(polinomio));

                raizesDerivada.insert(-std::numeric_limits<double>::infinity());
                raizesDerivada.insert(std::numeric_limits<double>::infinity());

                std::multiset<double>::const_iterator iter2 = raizesDerivada.cbegin();
                std::multiset<double>::const_iterator iter1 = iter2++;

                double value1, value2 = evaluatePolynomial(polinomio, *iter2);


                while(iter2 != raizesDerivada.cend()){

                    value1 = value2;
                    value2 = evaluatePolynomial(polinomio, *iter2);

                    if((value1 <= 0.0 && value2 >= 0.0) || (value1 >= 0.0 && value2 <= 0.0)){

                        double r1 = boundedBitSearch(polynomialEvaluator(polinomio), 0.0, *iter1, *iter2);
                        roots = raizes(polynomialDivision( polinomio, std::vector<double>{-r1, 1.0} ));
                        roots.insert(r1);

                        iter2 = raizesDerivada.cend();

                    }else{

                        iter1++;
                        iter2++;

                    }

                }

    	    }
            break;

    }

    return roots;

}

std::multiset<double> baskhara(std::vector<double> polinomio){

    std::multiset<double> roots;

    double delta = std::pow(polinomio[1], 2) - (4.0 * polinomio[0] * polinomio[2]);

    if(delta >= 0){
    	delta = std::sqrt(delta);
    	roots.insert( - (polinomio[1] + delta) / (2.0 * polinomio[2]) );
    	roots.insert( - (polinomio[1] - delta) / (2.0 * polinomio[2]) );

    }

    return roots;

}

std::vector<double> polynomialDivision(std::vector<double> dividend, std::vector<double> divisor){

    if(divisor.size() > dividend.size()) return std::vector<double>{};

    std::vector<double> quotient(dividend.size() - divisor.size() + 1);

    for(long long i = 1; i <= quotient.size(); i++){

        quotient[quotient.size() - i] = dividend[dividend.size() - i] / divisor[divisor.size() - 1];

        for(long long j = 2; j <= divisor.size(); j++){

            dividend[dividend.size() - i - j + 1] -= quotient[quotient.size() - i] * divisor[divisor.size() - j];

        }

    }

    return quotient;

}

std::vector<double> deriva (std::vector<double> polinomio){

    std::vector<double> derivada(std::max(polinomio.size(), 1ul) - 1);

    for(unsigned long i = 0; i < derivada.size(); i++)
        derivada[i] = polinomio[i + 1] * static_cast<double>(i + 1);

    return derivada;

}

double boundedBitSearch(std::function<double(double)> func, double target, double minimum, double maximum){

    if(!(minimum <= maximum)){

        printf("ERRO: Bounded Bit Search com minimo > maximo ou NaN\n");
        fflush(stdout);

        return 0.0;

    }

    if(func(minimum) > func(maximum)){
        func = negatefunction(func);
        target = -target;
    }

    double ans;
    double sign;

    if(func(0.0) <= target)
        sign = 1.0;
    else
        sign = -1.0;

    if(minimum < 0.0 && maximum >= 0.0){

        if(sign < 0.0){

            maximum = -minimum;
            minimum = 0.0;

        }else{

            minimum = 0.0;

        }

    }else{

        if(minimum < 0.0){

            std::swap(minimum, maximum);
            minimum = -minimum;
            maximum = -maximum;

        }

    }

    unsigned long long L, R, M;

    target *= sign;

    std::memcpy(&L, &minimum, sizeof(minimum));
    std::memcpy(&R, &maximum, sizeof(maximum));

    while(L != R){

        M = (L + R) / 2;

        std::memcpy( &ans, &M, sizeof M );

        ans = func(ans * sign);

        if(ans*sign < target) L = M + 1;
        else R = M;

    }

    std::memcpy( &ans, &M, sizeof M );
    ans *= sign;

    return ans;

}


std::vector<double> multiplyPolinomials(std::vector<double> p1, std::vector<double> p2){

    std::vector<double> ans(p1.size() + p2.size() - 1, 0.0);

    for(unsigned long i = 0; i < p1.size(); i++){

        for(unsigned long j = 0; j < p2.size(); j++){

            ans[i + j] += p1[i] * p2[j];

        }

    }

    return ans;

}

std::function<double(double)> negatefunction(std::function<double(double)> func){

    return [func](double f){return -func(f);};

}

std::function<double(double)> polynomialEvaluator(std::vector<double> polinomio){

    return [polinomio](double t){

        if(polinomio.size() == 0ul)
            return 0.0;

        if(t == 0.0 || t == -0.0)
            return polinomio[0];

        double ans = 0.0;

        for(long long i = polinomio.size() - 1; i >= 0; i--){

            ans += std::pow(t, i) * polinomio[i];

            if(std::isinf(ans)) return ans;

        }

    return ans;

    };

}

double evaluatePolynomial(std::vector<double> polinomio, double t){

    if(polinomio.size() == 0ul)
        return 0.0;

    if(t == 0.0 || t == -0.0)
        return polinomio[0];

    double ans = 0.0;

    for(long long i = polinomio.size() - 1; i >= 0; i--){

        ans += std::pow(t, i) * polinomio[i];

        if(std::isinf(ans)) return ans;

    }

    return ans;

}
