#include <iostream>
#include <format>
#include "option.h"
#include "pricing/closed_form.h"
#include "pricing/simulation.h"
#include "pricing/greeks.h"

int main(){
    
    // Test cases
    // ---- closed form ----

    Option eurCall(252, 0.20, 0.05, 100.0, 100.0,
                   OptionsType::European, OptionsSide::Call, UnderLyingType::Stock);
    std::cout << std::format("Eur Call ATM:        {:.4f}\n", calculateOptionsPriceClosedForm(eurCall));

    Option eurPut(252, 0.20, 0.05, 100.0, 100.0,
                  OptionsType::European, OptionsSide::Put, UnderLyingType::Stock);
    std::cout << std::format("Eur Put ATM:         {:.4f}\n", calculateOptionsPriceClosedForm(eurPut));

    // K=150, should be nearly worthless
    Option eurCallOTM(252, 0.20, 0.05, 100.0, 150.0,
                      OptionsType::European, OptionsSide::Call, UnderLyingType::Stock);
    std::cout << std::format("Eur Call OTM:        {:.4f}\n", calculateOptionsPriceClosedForm(eurCallOTM));

    // K=50, delta near 1
    Option eurCallITM(252, 0.20, 0.05, 100.0, 50.0,
                      OptionsType::European, OptionsSide::Call, UnderLyingType::Stock);
    std::cout << std::format("Eur Call ITM:        {:.4f}\n", calculateOptionsPriceClosedForm(eurCallITM));

    // no div => should match eur call
    Option amCallNoDivid(252, 0.20, 0.05, 100.0, 100.0,
                         OptionsType::American, OptionsSide::Call, UnderLyingType::Stock);
    std::cout << std::format("Amer Call (no div):  {:.4f}\n", calculateOptionsPriceClosedForm(amCallNoDivid));

    // div of 3.0 at t=30 => early exercise premium kicks in
    Option amCallDiv(252, 0.20, 0.05, 100.0, 100.0, 3.0, 30,
                     OptionsType::American, OptionsSide::Call, UnderLyingType::Stock);
    std::cout << std::format("Amer Call (div):     {:.4f}\n", calculateOptionsPriceClosedForm(amCallDiv));

    // S=100, K=90 ITM put, american >= european
    Option amPut(252, 0.20, 0.05, 90.0, 100.0,
                 OptionsType::American, OptionsSide::Put, UnderLyingType::Stock);
    std::cout << std::format("Amer Put ITM:        {:.4f}\n", calculateOptionsPriceClosedForm(amPut));

    // black-76
    Option futuresFutCall(252, 0.20, 0.05, 100.0, 100.0,
                          OptionsType::European, OptionsSide::Call,
                          UnderLyingType::FuturesWithFuturesSettlement);
    std::cout << std::format("Futures/Futures Call:{:.4f}\n", calculateOptionsPriceClosedForm(futuresFutCall));

    // garman-kohlhagen, rf=0.03
    Option fxCall(252, 0.08, 0.05, 1.10, 1.10, 0.03,
                  OptionsType::European, OptionsSide::Call, UnderLyingType::Forex);
    std::cout << std::format("Forex Call ATM:      {:.4f}\n", calculateOptionsPriceClosedForm(fxCall));


    // ---- monte carlo (252 steps, 100k paths) ----
    std::cout << "\n--- Monte Carlo ---\n";

    std::cout << std::format("MC Eur Call ATM:         {:.4f}\n", simulateOptionsPrice(eurCall,        252, 100000));
    std::cout << std::format("MC Eur Put ATM:          {:.4f}\n", simulateOptionsPrice(eurPut,         252, 100000));
    std::cout << std::format("MC Eur Call OTM:         {:.4f}\n", simulateOptionsPrice(eurCallOTM,     252, 100000));
    std::cout << std::format("MC Eur Call ITM:         {:.4f}\n", simulateOptionsPrice(eurCallITM,     252, 100000));
    std::cout << std::format("MC Amer Call (no div):   {:.4f}\n", simulateOptionsPrice(amCallNoDivid,  252, 100000));
    std::cout << std::format("MC Amer Call (div):      {:.4f}\n", simulateOptionsPrice(amCallDiv,      252, 100000));
    std::cout << std::format("MC Amer Put ITM:         {:.4f}\n", simulateOptionsPrice(amPut,          252, 100000));

    // asian is cheaper than vanilla due to averaging reducing effective vol
    Option asianCall(252, 0.20, 0.05, 100.0, 100.0, OptionsType::Asian, OptionsSide::Call, UnderLyingType::Stock);
    Option asianPut (252, 0.20, 0.05, 100.0, 100.0, OptionsType::Asian, OptionsSide::Put,  UnderLyingType::Stock);
    std::cout << std::format("MC Asian Call ATM:       {:.4f}\n", simulateOptionsPrice(asianCall, 252, 100000));
    std::cout << std::format("MC Asian Put ATM:        {:.4f}\n", simulateOptionsPrice(asianPut,  252, 100000));

    // bermuda: eur <= bermuda <= american
    Option bermudaCall(252, 0.20, 0.05, 100.0, 100.0, 3.0, 30, OptionsType::Bermuda, OptionsSide::Call, UnderLyingType::Stock);
    Option bermudaPut (252, 0.20, 0.05, 100.0, 100.0, 3.0, 30, OptionsType::Bermuda, OptionsSide::Put,  UnderLyingType::Stock);
    std::cout << std::format("MC Bermuda Call ATM:     {:.4f}\n", simulateOptionsPrice(bermudaCall, 252, 100000));
    std::cout << std::format("MC Bermuda Put ATM:      {:.4f}\n", simulateOptionsPrice(bermudaPut,  252, 100000));


    // ---- greeks (european only, analytic) ----
    std::cout << "\n--- Greeks ---\n";

    // delta: b=r => e^(b-r)T=1, so just N(d1)
    std::cout << std::format("Delta Eur Call ATM:      {:.4f}  (~0.637)\n",  delta(eurCall));
    std::cout << std::format("Delta Eur Put ATM:       {:.4f}  (~-0.363)\n", delta(eurPut));
    std::cout << std::format("Delta Eur Call ITM:      {:.4f}  (~0.9999)\n", delta(eurCallITM));  // K=50, very deep
    std::cout << std::format("Delta Eur Call OTM:      {:.4f}  (~0.047)\n",  delta(eurCallOTM));  // K=150, very deep

    // gamma same for calls/puts
    std::cout << std::format("Gamma Eur Call ATM:      {:.4f}  (~0.019)\n",  gamma(eurCall));
    std::cout << std::format("Gamma Eur Call ITM:      {:.4f}  (~0.000)\n",  gamma(eurCallITM));

    std::cout << std::format("Theta Eur Call ATM:      {:.4f}  (~-0.0176)\n", theta(eurCall));
    std::cout << std::format("Theta Eur Put ATM:       {:.4f}  (~-0.0045)\n", theta(eurPut));

    // vega same for calls/puts
    std::cout << std::format("Vega Eur Call ATM:       {:.4f}  (~0.375)\n",  vega(eurCall));
    std::cout << std::format("Vega Eur Put ATM:        {:.4f}  (~0.375)\n",  vega(eurPut));
    std::cout << std::format("Vega Eur Call OTM:       {:.4f}  (~0.097)\n",  vega(eurCallOTM));

    // rho: stock options use analytic formula, futures (b=0) falls back to MC
    std::cout << std::format("Rho Eur Call ATM:        {:.4f}  (~0.532)\n",  rho(eurCall, 252, 100000));
    std::cout << std::format("Rho Eur Put ATM:         {:.4f}  (~-0.419)\n", rho(eurPut, 252, 100000));
    std::cout << std::format("Rho Futures/Fut Call:    {:.4f}  (~-0.078)\n", rho(futuresFutCall, 252, 100000));


    // ---- simulated greeks (MC finite-difference bumping, 252 steps, 100k paths) ----
    std::cout << "\n--- Simulated Greeks (Monte Carlo) ---\n";

    // --- simulated delta ---
    std::cout << std::format("Sim Delta Eur Call ATM:  {:.4f}  (analytic ~{:.4f})\n",
        simulateDelta(eurCall,    252, 100000), delta(eurCall));
    std::cout << std::format("Sim Delta Eur Put ATM:   {:.4f}  (analytic ~{:.4f})\n",
        simulateDelta(eurPut,     252, 100000), delta(eurPut));
    std::cout << std::format("Sim Delta Eur Call ITM:  {:.4f}  (analytic ~{:.4f})\n",
        simulateDelta(eurCallITM, 252, 100000), delta(eurCallITM));
    std::cout << std::format("Sim Delta Eur Call OTM:  {:.4f}  (analytic ~{:.4f})\n",
        simulateDelta(eurCallOTM, 252, 100000), delta(eurCallOTM));

    // --- simulated gamma ---
    std::cout << std::format("Sim Gamma Eur Call ATM:  {:.4f}  (analytic ~{:.4f})\n",
        simulateGamma(eurCall,    252, 100000), gamma(eurCall));
    std::cout << std::format("Sim Gamma Eur Put ATM:   {:.4f}  (analytic ~{:.4f})\n",
        simulateGamma(eurPut,     252, 100000), gamma(eurPut));
    std::cout << std::format("Sim Gamma Eur Call ITM:  {:.4f}  (analytic ~{:.4f})\n",
        simulateGamma(eurCallITM, 252, 100000), gamma(eurCallITM));

    // --- simulated theta ---
    std::cout << std::format("Sim Theta Eur Call ATM:  {:.4f}  (analytic ~{:.4f})\n",
        simulateTheta(eurCall,    252, 100000), theta(eurCall));
    std::cout << std::format("Sim Theta Eur Put ATM:   {:.4f}  (analytic ~{:.4f})\n",
        simulateTheta(eurPut,     252, 100000), theta(eurPut));

    // --- simulated vega ---
    std::cout << std::format("Sim Vega Eur Call ATM:   {:.4f}  (analytic ~{:.4f})\n",
        simulateVega(eurCall,     252, 100000), vega(eurCall));
    std::cout << std::format("Sim Vega Eur Put ATM:    {:.4f}  (analytic ~{:.4f})\n",
        simulateVega(eurPut,      252, 100000), vega(eurPut));
    std::cout << std::format("Sim Vega Eur Call OTM:   {:.4f}  (analytic ~{:.4f})\n",
        simulateVega(eurCallOTM,  252, 100000), vega(eurCallOTM));

    // --- simulated rho ---
    std::cout << std::format("Sim Rho Eur Call ATM:    {:.4f}  (analytic ~{:.4f})\n",
        simulateRho(eurCall,          252, 100000), rho(eurCall, 252, 100000));
    std::cout << std::format("Sim Rho Eur Put ATM:     {:.4f}  (analytic ~{:.4f})\n",
        simulateRho(eurPut,           252, 100000), rho(eurPut, 252, 100000));
    std::cout << std::format("Sim Rho Futures/Fut Call:{:.4f}  (analytic ~{:.4f})\n",
        simulateRho(futuresFutCall,   252, 100000), rho(futuresFutCall, 252, 100000));

    // --- simulated phi (forex only) ---
    Option fxPut(252, 0.08, 0.05, 1.10, 1.10, 0.03,
                 OptionsType::European, OptionsSide::Put, UnderLyingType::Forex);
    std::cout << std::format("Sim Phi Forex Call ATM:  {:.4f}  (analytic ~{:.4f})\n",
        simulatePhi(fxCall, 252, 100000), phi(fxCall));
    std::cout << std::format("Sim Phi Forex Put ATM:   {:.4f}  (analytic ~{:.4f})\n",
        simulatePhi(fxPut,  252, 100000), phi(fxPut));

    return 0;
}