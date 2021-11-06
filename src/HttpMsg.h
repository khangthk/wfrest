#ifndef _HTTPMSG_H_
#define _HTTPMSG_H_

#include "workflow/HttpMessage.h"
#include "workflow/WFTaskFactory.h"
#include <fcntl.h>
#include <unordered_map>
#include "HttpDef.h"
#include "HttpContent.h"

namespace wfrest
{
    using RouteParams = std::unordered_map<std::string, std::string>;
    using QueryParams = std::unordered_map<std::string, std::string>;

    class HttpReq : public protocol::HttpRequest
    {
    public:
        // chunked body
        std::string Body() const;

        // body -> structured content
        void parse_body();

        // /{name}/{id} parans in route
        void set_route_params(RouteParams &&params)
        { route_params_ = std::move(params); }

        std::string param(const std::string &key)
        { return route_params_[key]; };

        // handler define path
        std::string full_path() const
        { return route_full_path_; }

        void set_full_path(const std::string &route_full_path)
        { route_full_path_ = route_full_path; }

        // parser uri
        void set_parsed_uri(ParsedURI &&parsed_uri)
        { parsed_uri_ = std::move(parsed_uri); }

        // url query params
        void set_query_params(QueryParams &&query_params)
        { query_params_ = std::move(query_params); }

        std::string query(const std::string &key)
        { return query_params_[key]; }

        std::string default_query(const std::string &key, const std::string &default_val);

        template<typename T>
        T param(const std::string &key) const;

        QueryParams query_list() const
        { return query_params_; }

        bool has_query(const std::string &key);

    private:
        void fill_content_type();

    public:
        Urlencode::KV kv;
        MultiPartForm::MultiPart form;
        http_content_type content_type;
    private:
        RouteParams route_params_;
        std::string route_full_path_;
        ParsedURI parsed_uri_;
        QueryParams query_params_;
        MultiPartForm multi_part_;
    };

    template<>
    inline int HttpReq::param<int>(const std::string &key) const
    {
        if (route_params_.count(key))
            return std::stoi(route_params_.at(key));
        else
            return 0;
    }

    template<>
    inline size_t HttpReq::param<size_t>(const std::string &key) const
    {
        if (route_params_.count(key))
            return static_cast<size_t>(std::stoul(route_params_.at(key)));
        else
            return 0;
    }

    template<>
    inline double HttpReq::param<double>(const std::string &key) const
    {
        if (route_params_.count(key))
            return std::stod(route_params_.at(key));
        else
            return 0.0;
    }


    class HttpResp : public protocol::HttpResponse
    {
    public:
        void String(const std::string &str);

        void String(std::string &&str);

        void String(const char *data, size_t len);

        // todo : json / file clear_output_body
        void File(const std::string &path);

        // void Write(const std::string& content, const std::string& path);

        void set_status(int status_code);

        void test()
        { fprintf(stderr, "resp test : %s\n", get_status_code()); }
    };

    using WebTask = WFNetworkTask<HttpReq, HttpResp>;

} // namespace wfrest


#endif // _HTTPMSG_H_