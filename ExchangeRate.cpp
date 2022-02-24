#include <iostream>
#include <string>
#include <locale>
#include <cstdlib>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;

constexpr auto EXC_RATE_API = "https://api.coinbase.com/v2/exchange-rates?currency=";
constexpr auto CURRENCIES_API = "https://api.coinbase.com/v2/currencies";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

string curlData(string url = "") {

	//cout << "CURL URL: " << url << endl;

	CURL* curl;
	string readBuffer;

	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return readBuffer;
	}
	else {
		return "";
	}
}

void getCurrencies() {
	string res = curlData(CURRENCIES_API);
	if (res.length() > 0) {
		auto json = nlohmann::json::parse(res);
		cout << "\n List of available currencies" << endl;
		cout << "----------------------------------------------" << endl;
		cout << " Code - Name " << endl;
		cout << "----------------------------------------------" << endl;

		for (int i = 0; i < json["data"].size(); i++) {
			cout << json["data"][i]["id"] << " - " << json["data"][i]["name"] << endl;
		}
	}
}

void getExchangeRates()
{
}

void getExchangeRates(string baseCurrency = "BTC") {
	transform(baseCurrency.begin(), baseCurrency.end(), baseCurrency.begin(), ::toupper);

	string res = curlData(EXC_RATE_API + baseCurrency);

	if (res.length() > 0) {
		auto json = nlohmann::json::parse(res);
		cout << json["data"].dump(2) << endl;
	}
}

void getExchangeRate(string currencyFrom = "BTC", string currencyTo = "USD", float exchangeAmount = 1.00) {

	transform(currencyFrom.begin(), currencyFrom.end(), currencyFrom.begin(), ::toupper);
	transform(currencyTo.begin(), currencyTo.end(), currencyTo.begin(), ::toupper);

	string res = curlData(EXC_RATE_API + currencyFrom);
	if (res.length() > 0) {
		auto json = nlohmann::json::parse(res);

		string rateto = json["data"]["rates"][currencyTo.c_str()];
		float rate_to = stof(rateto);
		float exchangeTotal = exchangeAmount * rate_to;

		cout << "Currency From (" + currencyFrom + "): " << fixed << setprecision(2) << exchangeAmount << endl;
		cout << "Currency To (" + currencyTo + "): " << fixed << setprecision(2) << exchangeTotal << endl;
	}
}
