# -*- coding: utf-8 -*-
# try something like
import datetime
import json
import collections
import cgi

def index():
    response = {
        'success': 'false',
        'message': 'This controller method has no information :-('
    }
    return json.dumps(response)

def experiment_form():
    update = db.experiment(request.args(0))
    form = SQLFORM(db.experiment, update)
    if form.accepts(request,session):
        response.flash = 'Thanks! The form has been submitted.'
    elif form.errors:
       response.flash = 'Please correct the error(s).'
    else:
       response.flash = 'Try again - no fields can be empty.'
    return dict(form=form)

def all_records():
    grid = SQLFORM.grid(db.experiment,user_signature=False)
    return locals()

def get_experiment():
    server_name = request.vars.server_name
    myrecord = db(db.experiment.status == "READY").select(
        orderby=db.experiment.id
    ).first()

    if(myrecord != None):
        db(db.experiment.id == myrecord.id).update(server_executing=server_name, ip=request.client, status="REQUESTED", last_modified_date=datetime.datetime.now())

        xml_data = myrecord.xml_data
        xml_data = xml_data.replace("\"", "'")

        xml_data = cgi.escape(xml_data)

        d = collections.OrderedDict()
        d['success'] = 'true'
        d['id'] = myrecord.id
        d['xml_data'] = xml_data
        d['status'] = myrecord.status
        d['name'] = myrecord.name
        d['num_matches'] = myrecord.num_matches
        d['position_number'] = myrecord.position_number
        d['created_date'] = myrecord.created_date.strftime("%Y-%m-%d %H:%M:%S")
        d['last_modified_date'] = myrecord.last_modified_date.strftime("%Y-%m-%d %H:%M:%S")

        response.headers['Content-Type'] = 'application/json'
        return json.dumps(d)
    else:
        json_response = {
            'success': 'false',
            'message': 'no more values'
        }
        response.headers['Content-Type'] = 'application/json'
        return json.dumps(json_response)


def set_experiment_status():
    experiment_id = request.vars.experiment_id
    experiment_status = request.vars.experiment_status

    json_response = {}

    if not experiment_id or not experiment_status:
        json_response['success'] = 'false'
        json_response['message'] = 'id or status are missing'
        response.headers['Content-Type'] = 'application/json'
        return json.dumps(json_response)

    update_result = db(db.experiment.id == experiment_id).update(status=experiment_status, last_modified_date=datetime.datetime.now())
    #return dict(message=update_result)

    if update_result == 1:
        json_response['success'] = 'true'
        json_response['message'] = 'updated value with success'
    else:
        json_response['success'] = 'false'
        json_response['message'] = 'cant update record'

    response.headers['Content-Type'] = 'application/json'
    return json.dumps(json_response)

