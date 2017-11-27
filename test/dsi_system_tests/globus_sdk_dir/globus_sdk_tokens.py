# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/globus_sdk_dir/globus_sdk_tokens.py $: $id:$ $Rev: 17 $
import globus_sdk
import test_config

class globus_sdk_tokens(object):
    
    def authorize(self):
        client = globus_sdk.NativeAppAuthClient(test_config.client_id)
        client.oauth2_start_flow_native_app(refresh_tokens=True)
        authorize_url = client.oauth2_get_authorize_url()
        print('Please go to this URL and login: {0}'.format(authorize_url))
        auth_code = raw_input('Please enter the code you get after login here: ').strip()
        print 'Auth code: ' + auth_code
        token_response = client.oauth2_exchange_code_for_tokens(auth_code)
        print token_response
        # let's get stuff for the Globus Transfer service
        globus_transfer_data = token_response.by_resource_server['transfer.api.globus.org']
        # the refresh token and access token, often abbr. as RT and AT
        transfer_rt = globus_transfer_data['refresh_token']
        transfer_at = globus_transfer_data['access_token']
        expires_at_s = globus_transfer_data['expires_at_seconds']

        # Now we've got the data we need, but what do we do?
        # That "GlobusAuthorizer" from before is about to come to the rescue
        authorizer = globus_sdk.RefreshTokenAuthorizer(transfer_rt, client,
                access_token=transfer_at, expires_at=expires_at_s)
        return authorizer 
