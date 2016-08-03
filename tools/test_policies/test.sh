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
}

test_with_curl_data()
{
    ANSWER=`curl -s -X POST -H "Content-Type: application/json" -d "$2" "$1"`
    ./validate_json.py "$ANSWER" "$3"
    if test $? -eq 0; then
        echo "$4";
    else
        echo "command $1 failed"
        echo "$ANSWER"
        exit 1;
    fi;
}

#create XSLT policy (X2)
#get policies and policies count
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}' "pass get policies count"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": []}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/xslt_policy_create" '{"XSLT_POLICY_CREATE_RESULT": {"id": 0}}' "pass create new policy"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": [{"type": "and", "id": 0, "description": "", "parent_id": -1, "name": "New policy"}]}}' "pass get policies"
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
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": [{"type": "and", "parent_id": -1, "name": "changed name", "description": "changed description", "id": 0},{"type": "and", "id": 1, "description": "", "parent_id": -1, "name": "New policy 1"}]}}' "pass get policies"

#clear policies
test_with_curl "http://0.0.0.0:4242/1.7/policy_clear_policies" '{"POLICY_CLEAR_POLICIES_RESULT": {}}' "pass clear policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies" '{"POLICY_GET_POLICIES_RESULT": {"policies": []}}' "pass get policies"
test_with_curl "http://0.0.0.0:4242/1.7/policy_get_policies_count" '{"POLICY_GET_POLICIES_COUNT_RESULT": {"size": 0}}' "pass get policies count"