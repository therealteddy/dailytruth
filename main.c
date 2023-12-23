/* 
Author: Ted Josh Issac Jerin 
Description: Gives you a new word of the true god every day! 
Date: 22-Dec-23 
*/

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <curl/curl.h> 
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>

#define URL "https://www.christianity.com/bible/daily-bible-verse/"
#define URL_FILE "./www.christianity.com.html"

#define XPATH_VERSE_EXPR "//div[@class=\"row margin-top-20\"]//blockquote/text()" 
#define XPATH_LOCATION_EXPR "//div[@class=\"row margin-top-20\"]//div[@class=\"col-md-12\"]/h2/a[@href]/text()"
#define XPATH_BIBLE_VERSION_EXPR "//div[@class=\"row margin-top-20\"]//div[@class=\"col-md-12\"]/h2/a/small/text()"

#define CURL_TRUE 1L 
#define CURL_FALSE 0L 

size_t curl_write(void* dataptr, size_t size, size_t nmemb, void* userdata); 

int main(int argc, char** argv) {
    FILE* htmldoc; 
    CURL* curlctx; 
    htmlDocPtr html_file; 
    xmlXPathContextPtr xpath_ctx;
    xmlXPathObjectPtr xpath_verse; 
    xmlXPathObjectPtr xpath_location;
    xmlXPathObjectPtr xpath_bible_ver;

    // Set an encoding to prevent encoding/decoding error
    setlocale(LC_ALL, "en_US.UTF-8");

    // Initialize XML Parser
    xmlInitParser();

    // Create/Open a new html file 
    if (!(htmldoc = fopen(URL_FILE, "w+"))) {
        fprintf(stderr, "Failed to open file %s at line %d\n", URL_FILE, __LINE__); 
        abort();
    }

    // Initialize a new curl handle 
    if (!(curlctx = curl_easy_init())) {
        fprintf(stderr, "Faied to curl_easy_init() at %d\n", __LINE__); 
        abort();
    }

    // Set a url 
    curl_easy_setopt(curlctx, CURLOPT_URL, URL); 

    // Set up callback and pass user-defined value 
    curl_easy_setopt(curlctx, CURLOPT_WRITEFUNCTION, curl_write); 
    curl_easy_setopt(curlctx, CURLOPT_WRITEDATA, htmldoc); 

    // Disable SSL Checking
    curl_easy_setopt(curlctx, CURLOPT_SSL_VERIFYPEER, CURL_FALSE); 
    curl_easy_setopt(curlctx, CURLOPT_SSL_VERIFYHOST, CURL_FALSE);

    // Curl perform, handle errors and close fd to prevent the document being null (truncated to zero length) before being parsed to htmlReadFile()
    int curl_errcode;
    if ((curl_errcode = curl_easy_perform(curlctx)) != CURLE_OK) {
        printf("Failed to curl_easy_perform()\n%s\n", curl_easy_strerror(curl_errcode)); 
        abort();
    }
    fclose(htmldoc);

    // Parse html file 
    if(!(html_file = htmlReadFile(URL_FILE, "UTF-8", HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING))) {
        fprintf(stderr, "Failed to parse HTML File %s at line %d\n", URL_FILE, __LINE__); 
        abort();
    }

    // Create a new xpath context 
    if(!(xpath_ctx = xmlXPathNewContext(html_file))) {
        fprintf(stderr, "Failed to create XPath context at line %d\n", __LINE__); 
        abort();
    }

    // Extract the Bible Verse, Location, and Bible Version  
    xpath_verse = xmlXPathEvalExpression(XPATH_VERSE_EXPR, xpath_ctx); 
    xpath_location = xmlXPathEvalExpression(XPATH_LOCATION_EXPR, xpath_ctx); 
    xpath_bible_ver = xmlXPathEvalExpression(XPATH_BIBLE_VERSION_EXPR, xpath_ctx); 

    if (!(xpath_verse) || !(xpath_location) || !(xpath_bible_ver)) {
        fprintf(stderr, "Failed to evaluate one or more XPath expressions at line %d\n", __LINE__); 
        abort();
    }

    // Print the extracted information
    printf("%s", xpath_verse->nodesetval->nodeTab[0]->content); // The verse
    printf("%s ", xpath_location->nodesetval->nodeTab[0]->content); // ... Followed by the location
    printf("(%s)\n", xpath_bible_ver->nodesetval->nodeTab[0]->content);  // ... and the the version in parenthesis

    // Perform all cleanup and exit!
    xmlXPathFreeObject(xpath_verse);
    xmlXPathFreeObject(xpath_location);
    xmlXPathFreeObject(xpath_bible_ver);
    
    xmlXPathFreeContext(xpath_ctx);
    xmlFreeDoc((xmlDocPtr) html_file); 
    
    curl_easy_cleanup(curlctx);
    exit(EXIT_SUCCESS);

}

size_t curl_write(void* dataptr, size_t size, size_t nmemb, void* userdata) {
    return (size_t) ((((FILE*) userdata) == NULL) ? fwrite((char*) dataptr, size, nmemb, stdout) : fwrite((char*) dataptr, size, nmemb, (FILE*) userdata));
}