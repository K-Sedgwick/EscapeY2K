import json

# This is actually a dictionary
student_details = { 
    "name" : "EscapeY2K", 
    "phonenumber" : "Wouldn't you like to know?"
} 

# I can add stuff that didnt exist before
student_details['foo'] = 234
# and I can update stuff/change its datatype
student_details['phonenumber'] = "Its a secred ;)"

print(json.dumps(student_details))