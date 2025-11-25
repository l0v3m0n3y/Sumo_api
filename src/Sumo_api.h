#ifndef SUMO_API_H
#define SUMO_API_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Sumo_api {
private:
    std::string api_base = "https://www.sumo-api.com/api";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("www.sumo-api.com"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }
public:
    Sumo_api(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> get_rikishis(
        const std::string& shikonaEn = "",
        const std::string& heya = "",
        const std::string& sumodbId = "",
        const std::string& nskId = "",
        const std::string& intai = "",
        const std::string& measurements = "",
        const std::string& ranks = "",
        const std::string& shikonas = "",
        int limit = 0,
        int skip = 0) {
        
        std::map<std::string, std::string> params;
        if (!shikonaEn.empty()) params["shikonaEn"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(shikonaEn)));
        if (!heya.empty()) params["heya"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(heya)));
        if (!sumodbId.empty()) params["sumodbId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sumodbId)));
        if (!nskId.empty()) params["nskId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(nskId)));
        if (!intai.empty()) params["intai"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(intai)));
        if (!measurements.empty()) params["measurements"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(measurements)));
        if (!ranks.empty()) params["ranks"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(ranks)));
        if (!shikonas.empty()) params["shikonas"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(shikonas)));
        if (limit > 0) params["limit"] = std::to_string(limit);
        if (skip > 0) params["skip"] = std::to_string(skip);
        
        return make_api_call("/rikishis" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_rikishi_by_id(
        const std::string& rikishiId,
        const std::string& measurements = "",
        const std::string& ranks = "",
        const std::string& shikonas = "") {
        
        std::map<std::string, std::string> params;
        if (!measurements.empty()) params["measurements"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(measurements)));
        if (!ranks.empty()) params["ranks"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(ranks)));
        if (!shikonas.empty()) params["shikonas"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(shikonas)));
        
        return make_api_call("/rikishi/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId))) + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_rikishi_stats(const std::string& rikishiId) {
        return make_api_call("/rikishi/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId))) + "/stats", "GET");
    }

    pplx::task<json::value> get_rikishi_matches(
        const std::string& rikishiId,
        const std::string& bashoId = "") {
        
        std::map<std::string, std::string> params;
        if (!bashoId.empty()) params["bashoId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId)));
        
        return make_api_call("/rikishi/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId))) + "/matches" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_rikishi_matches_vs_opponent(
        const std::string& rikishiId,
        const std::string& opponentId,
        const std::string& bashoId = "") {
        
        std::map<std::string, std::string> params;
        if (!bashoId.empty()) params["bashoId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId)));
        
        return make_api_call("/rikishi/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId))) + "/matches/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(opponentId))) + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_basho(const std::string& bashoId) {
        return make_api_call("/basho/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId))), "GET");
    }

    pplx::task<json::value> get_basho_banzuke(
        const std::string& bashoId,
        const std::string& division) {
        
        return make_api_call("/basho/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId))) + "/banzuke/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(division))), "GET");
    }

    pplx::task<json::value> get_basho_torikumi(
        const std::string& bashoId,
        const std::string& division,
        int day) {
        
        return make_api_call("/basho/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId))) + "/torikumi/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(division))) + "/" + std::to_string(day), "GET");
    }

    pplx::task<json::value> get_kimarite(
        const std::string& sortField = "",
        const std::string& sortOrder = "",
        int limit = 0,
        int skip = 0) {
        
        std::map<std::string, std::string> params;
        if (!sortField.empty()) params["sortField"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sortField)));
        if (!sortOrder.empty()) params["sortOrder"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sortOrder)));
        if (limit > 0) params["limit"] = std::to_string(limit);
        if (skip > 0) params["skip"] = std::to_string(skip);
        
        return make_api_call("/kimarite" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_kimarite_matches(
        const std::string& kimarite,
        const std::string& sortOrder = "",
        int limit = 0,
        int skip = 0) {
        
        std::map<std::string, std::string> params;
        if (!sortOrder.empty()) params["sortOrder"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sortOrder)));
        if (limit > 0) params["limit"] = std::to_string(limit);
        if (skip > 0) params["skip"] = std::to_string(skip);
        
        return make_api_call("/kimarite/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(kimarite))) + build_query_params(params), "GET");
    }


    pplx::task<json::value> get_measurements(
        const std::string& rikishiId = "",
        const std::string& bashoId = "",
        const std::string& sortOrder = "") {
        
        std::map<std::string, std::string> params;
        if (!rikishiId.empty()) params["rikishiId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId)));
        if (!bashoId.empty()) params["bashoId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId)));
        if (!sortOrder.empty()) params["sortOrder"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sortOrder)));
        
        return make_api_call("/measurements" + build_query_params(params), "GET");
    }


    pplx::task<json::value> get_ranks(
        const std::string& rikishiId = "",
        const std::string& bashoId = "",
        const std::string& sortOrder = "") {
        
        std::map<std::string, std::string> params;
        if (!rikishiId.empty()) params["rikishiId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId)));
        if (!bashoId.empty()) params["bashoId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId)));
        if (!sortOrder.empty()) params["sortOrder"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sortOrder)));
        
        return make_api_call("/ranks" + build_query_params(params), "GET");
    }


    pplx::task<json::value> get_shikonas(
        const std::string& rikishiId = "",
        const std::string& bashoId = "",
        const std::string& sortOrder = "") {
        
        std::map<std::string, std::string> params;
        if (!rikishiId.empty()) params["rikishiId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(rikishiId)));
        if (!bashoId.empty()) params["bashoId"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bashoId)));
        if (!sortOrder.empty()) params["sortOrder"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(sortOrder)));
        
        return make_api_call("/shikonas" + build_query_params(params), "GET");
    }
};

#endif
