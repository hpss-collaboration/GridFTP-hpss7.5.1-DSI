# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/globus_sdk_dir/globus_sdk_transfer.py $: $id:$ $Rev: 17 $
import globus_sdk
from globus_sdk_tokens import globus_sdk_tokens

import logging


class globus_sdk_transfer(object):

    def __init__(self, authorizer):
        transfer_client = globus_sdk.TransferClient(authorizer=authorizer)
        self.tc = transfer_client

    def transfer_file(self, source_ep, dest_ep, source_path, dest_path):

        logging.info('Transfering from source endpoint:%s path:%s to '
                + 'destination endpoint:%s path:%s', source_ep, source_path,
                dest_ep, dest_path)
        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep)
        tdata.add_item(source_path, dest_path)
        return self.tc.submit_transfer(tdata)

    def transfer_file_preserve_timestamp(self, source_ep, dest_ep,
            source_path, dest_path):
        logging.info('Transfering from source endpoint:%s path:%s to '
                + 'destination endpoint:%s path:%s', source_ep, source_path,
                dest_ep, dest_path + ' with preserve timestamp option')
        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                preserve_timestamp=True)
        tdata.add_item(source_path, dest_path)
        return self.tc.submit_transfer(tdata)
    
    def transfer_file_checksum_option(self, source_ep, dest_ep, source_path,
            dest_path):

        logging.info('Transfering from source endpoint:%s path:%s to ' +
                'destination endpoint:%s path:%s with checksum verification',
                source_ep, source_path, dest_ep, dest_path)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                verify_checksum=True)
        tdata.add_item(source_path, dest_path)
        return self.tc.submit_transfer(tdata)

    def transfer_file_sync_option(self, source_ep, dest_ep, source_path,
            dest_path, sync_level):

        logging.info('Transfering from source endpoint:%s path:%s to '
                + 'destination endpoint:%s path:%s with sync option level %s',
                source_ep, source_path, dest_ep, dest_path, sync_level)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                sync_level=sync_level)
        tdata.add_item(source_path, dest_path)
        return self.tc.submit_transfer(tdata)

    def transfer_file_checksum_sync_option(self, source_ep, dest_ep,
            source_path, dest_path, sync_level):

        logging.info('Transfering from source endpoint:%s path:%s to '
                + 'destination endpoint:%s path:%s with checksum ' +
                'verification and sync option %s', source_ep, source_path,
                dest_ep, dest_path, sync_level)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                verify_checksum=True, sync_level=sync_level)
        tdata.add_item(source_path, dest_path)
        return self.tc.submit_transfer(tdata)

    def recursive_transfer_file(self, source_ep, dest_ep, source_path,
            dest_path):

        logging.info('Recursively transfering from source endpoint:%s ' +
                'path:%s to destination endpoint:%s path:%s',
                source_ep, source_path, dest_ep, dest_path)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep)
        tdata.add_item(source_path, dest_path, recursive=True)
        return self.tc.submit_transfer(tdata)

    def recursive_transfer_file_delete_extra(self, source_ep, dest_ep,
            source_path, dest_path):
        logging.info('Recursively transfering from source endpoint:%s ' +
                'path:%s to destination endpoint:%s path:%s with delete ' +
                'extra files option',
                source_ep, source_path, dest_ep, dest_path)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                delete_destination_extra=True)
        tdata.add_item(source_path, dest_path, recursive=True)
        return self.tc.submit_transfer(tdata)

    def recursive_transfer_file_checksum_option(self, source_ep, dest_ep,
            source_path, dest_path):

        logging.info('Recursively transfering from source endpoint:%s ' +
                'path:%s to destination endpoint:%s path:%s with checksum ' +
                'verification', source_ep, source_path, dest_ep, dest_path)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                verify_checksum=True)
        tdata.add_item(source_path, dest_path, recursive=True)
        return self.tc.submit_transfer(tdata)

    def recursive_transfer_file_sync_option(self, source_ep, dest_ep,
            source_path, dest_path, sync_level):

        logging.info('Recursively transfering from source endpoint:%s ' +
                'path:%s to destination endpoint:%s path:%s with sync ' +
                'option level %s', source_ep, source_path, dest_ep,
                dest_path, sync_level)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                sync_level=sync_level)
        tdata.add_item(source_path, dest_path, recursive=True)
        return self.tc.submit_transfer(tdata)

    def recursive_transfer_file_checksum_sync_option(self, source_ep, dest_ep,
           source_path, dest_path, sync_level):

        logging.info('Recursively transfering from source endpoint:%s ' +
                'path:%s to destination endpoint:%s path:%s with ' +
                'checksum verification and sync option %s', source_ep,
                source_path, dest_ep, dest_path, sync_level)

        tdata = globus_sdk.TransferData(self.tc, source_ep, dest_ep,
                verify_checksum=True, sync_level=sync_level)
        tdata.add_item(source_path, dest_path, recursive=True)
        return self.tc.submit_transfer(tdata)
