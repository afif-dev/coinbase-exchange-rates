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

string curlData(string url="") {

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

void getMenuOpt(int opt = 0) {
	
	float exchangeAmount = 1.00;
	string baseCurrency = "BTC";
	string currencyFrom = "BTC";
	string currencyTo = "USD";

	switch (opt) {
	case 0:
		cout << "\n\nMain Menu:" << endl;
		cout << "[1] - List of available currencies" << endl;
		cout << "[2] - Exchange rates by base currency" << endl;
		cout << "[3] - Exchange rate between 2 currency" << endl;
		cout << "[4] - Exit" << endl;
		cout << "Type a number: ";
		while (!(cin >> opt) || opt < 0 || opt > 4)
		{
			cout << "Error! - Invalid number" << endl;
			cout << "Type a number: ";
			cin.clear();
			cin.ignore(123, '\n');
		}
		if (opt > 0 ) {
			getMenuOpt(opt);
		}
		break;
	case 1:
		getCurrencies();
		getMenuOpt(0); // back to main menu
		break;
	case 2:
		cout << "\n\nExchange rates by base currency:" << endl;
		cout << "----------------------------------------------" << endl;
		cout << "Type a base currency code ("+ baseCurrency +"): ";
		cin >> baseCurrency;
		getExchangeRates(baseCurrency);
		getMenuOpt(0); // back to main menu
		break;
	case 3:
		cout << "\n\nExchange rate between 2 currency:" << endl;
		cout << "----------------------------------------------" << endl;
		cout << "Type a currency code 1 (" + currencyFrom + "): ";
		cin >> currencyFrom;
		cout << "Type a exchange amount (" << fixed << setprecision(2) << exchangeAmount << "): ";
		
		while (!(cin >> exchangeAmount))
		{
			cout << "Error! - Invalid exchange amount" << endl;
			cout << "Type a exchange amount (" << fixed << setprecision(2) << exchangeAmount << "): ";
			cin.clear();
			cin.ignore(123, '\n');
		}

		cout << "Type a currency code 2 (" + currencyTo + "): ";
		cin >> currencyTo;
		cout << "----------------------------------------------" << endl;
		if (currencyFrom.size() < 2 || currencyTo.size() < 2) {
			getExchangeRate();
		}
		else {
			getExchangeRate(currencyFrom, currencyTo, exchangeAmount);
		}
		getMenuOpt(0); // back to main menu
		break;
	case 4:
		exit(0);
		break;
	}
}

int main() {
	
	cout << "#######################################################################" << endl;
	cout << "* Exchange Rates" << endl;
	cout << "- Get current exchange rates base on Coinbase (https://www.coinbase.com/)" << endl;
	cout << "#######################################################################" << endl;

	getMenuOpt();
	
	cin.get();
	return 0;
}
