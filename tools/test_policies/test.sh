#!/bin/sh

test_with_curl()
{
    ANSWER=`curl -s "$1"`
    ./validate_json.py "$ANSWER" "$2"
    if test $? -eq 0; then
        echo "$3";
    else
        echo "command $1 failed"
        echo "$ANSWER"
        exit 1;
    fi;
    ANSWER=
}

test_with_curl_data()
{
    ANSWER=`curl -s -X POST -H "Content-Type: application/json" -d "$2" "$1"`
    if test $? -ne 0; then
        echo "command $1 failed"
        exit 1;
    fi;

    ./validate_json.py "$ANSWER" "$3"
    if test $? -eq 0; then
        echo "$4";
    else
        echo "command $1 failed"
        echo "$ANSWER"
        exit 1;
    fi;
    ANSWER=
}

#create XSLT policy (X2)
#get policies and policies count
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}' "pass get policies count"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": []}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_create" '{"XSLT_POLICY_CREATE_RESULT": {"id": 0}}' "pass create new policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get?id=0" '{"POLICY_GET_RESULT": {"policy": {"type": "and", "id": 0, "description": "", "parent_id": -1, "name": "New policy", "is_system": false}}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": [{"type": "and", "id": 0, "description": "", "parent_id": -1, "name": "New policy", "is_system": false}]}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 1}}' "pass get policies count"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_create" '{"XSLT_POLICY_CREATE_RESULT": {"id": 1}}' "pass create new policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 2}}' "pass get policies count"

#save policy
test_with_curl "http://0.0.0.0:4242/1.7/policy_save?id=0" '{"POLICY_SAVE_RESULT": {}}' "pass save policy"

#get/set policy name
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_name?id=0" '{"POLICY_GET_NAME_RESULT": {"name": "New policy"}}' "pass get name"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_name?id=1" '{"POLICY_GET_NAME_RESULT": {"name": "New policy 1"}}' "pass get name"
test_with_curl_data "http://0.0.0.0:4242/1.7/policy_change_name" '{"POLICY_CHANGE_NAME":{"id": 0, "name": "changed name", "description": "changed description"}}' '{"POLICY_CHANGE_NAME_RESULT": {}}' "pass change name"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_name?id=0" '{"POLICY_GET_NAME_RESULT": {"name": "changed name"}}' "pass get name"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": [{"type": "and", "parent_id": -1, "name": "changed name", "description": "changed description", "id": 0, "is_system": false},{"type": "and", "id": 1, "description": "", "parent_id": -1, "name": "New policy 1", "is_system": false}]}}' "pass get policies"

#clear policies
test_with_curl "http://0.0.0.0:4242/1.7/policy_clear_policies" '{"POLICY_CLEAR_POLICIES_RESULT": {}}' "pass clear policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": []}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}' "pass get policies count"

JSON_DATA=`cat ./data/sample_policy_1.xml | sed 's/"/\\\\"/g' | sed -e ':a' -e 'N' -e '$!ba' -e 's/\n//g'`

#import and remove
# import file has 8 policies & 16 rules
test_with_curl_data "http://0.0.0.0:4242/1.7/policy_import" "{\"POLICY_IMPORT\": {\"xml\": \"$JSON_DATA\"}}" '{"POLICY_IMPORT_RESULT": {"id": 2}}' "pass import policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_remove?id=2" '{"POLICY_REMOVE_RESULT": {}}' "pass remove policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}' "pass get policies count"

#dump and duplicate
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_create" '{"XSLT_POLICY_CREATE_RESULT": {"id": 11}}' "pass create new policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_dump?id=11" '{"POLICY_DUMP_RESULT": {"xml": "<?xml version=\"1.0\"?>\n<policy type=\"and\" name=\"New policy\"/>\n"}}' "pass dump policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_duplicate?id=11" '{"POLICY_DUPLICATE_RESULT": {"id": 12}}' "pass duplicate policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_remove?id=11" '{"POLICY_REMOVE_RESULT": {}}' "pass remove policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": [{"type": "and", "id": 12, "description": "", "parent_id": -1, "name": "New policy_copy", "is_system": false}]}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_remove?id=12" '{"POLICY_REMOVE_RESULT": {}}' "pass remove policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}' "pass get policies count"
test_with_curl "http://0.0.0.0:4242/1.7/policy_clear_policies" '{"POLICY_CLEAR_POLICIES_RESULT": {}}' "pass clear policies"

# XSLT policy rule operations
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_create" '{"XSLT_POLICY_CREATE_RESULT": {"id": 13}}' "pass create new policy"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_create?policy_id=13" '{"XSLT_POLICY_RULE_CREATE_RESULT": {"id": 16}}' "pass create new xslt policy rule"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_get?policy_id=13&id=16" '{"XSLT_POLICY_RULE_GET_RESULT": {"rule": {"field": "", "id": 16, "occurrence": -1, "name": "New Rule", "ope": "", "tracktype": "", "value": ""}}}' "pass get xslt policy rule"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_duplicate?policy_id=13&id=16" '{"XSLT_POLICY_RULE_DUPLICATE_RESULT": {"id": 17}}' "pass duplicate xslt policy rule"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_get?policy_id=13&id=17" '{"XSLT_POLICY_RULE_GET_RESULT": {"rule": {"field": "", "id": 17, "occurrence": -1, "name": "New Rule_copy", "ope": "", "tracktype": "", "value": ""}}}' "pass get xslt policy rule"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_delete?policy_id=13&id=16" '{"XSLT_POLICY_RULE_DELETE_RESULT": {}}' "pass delete xslt policy rule"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_delete?policy_id=13&id=17" '{"XSLT_POLICY_RULE_DELETE_RESULT": {}}' "pass delete xslt policy rule"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_rule_delete?policy_id=13&id=16" '{"XSLT_POLICY_RULE_DELETE_RESULT": {"nok": {"error": "Policy rule does not exist"}}}' "pass delete unexisting xslt policy rule"
