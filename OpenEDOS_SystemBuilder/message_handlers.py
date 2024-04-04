"""
This is the OpenEDOS SystemBuilder v0.1.
(c) 2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

from typing import List, Dict, Set
from user_code import UserCode
import utils

class Parameter():
    def __init__(self, 
        name:str, 
        type:str, 
        description:str
        ) -> None:
        self.name = name
        self.type = type
        self.description = description

    def get_comment_text(self) -> str:
        return f"@param {self.name} {self.description}"

    def get_prototype_text(self, pointer:bool=False) -> str:
        if pointer:
            text = f"\t{self.type} *{self.name}"
        else:
            text = f"\t{self.type} {self.name}"
        return text

class Sender():
    def __init__(self,
        name:str,
        func_name:str,
        type:str,
        brief:str,
        description:str="",
        parameters:Dict[str, Parameter]={},
        args:Dict[str, Parameter]={}
        ) -> None:
        self.name = name
        self.func_name = func_name
        self.type = type
        self.brief = brief
        if description != "" and description[-1] != "\n":
            description += "\n"
        self.description = description
        self.parameters = parameters
        self.args = args
        self.RID = f"RID_{name}"

    def _params(self) -> str:
        if len(self.parameters) == 0:
            text = "(void)"
            return text
        text = "("  
        for param in self.parameters.values():
            text += f"\n{param.get_prototype_text()},"
        text = f"{text[:-1]})"
        return text
    
    def _comment(self) -> str:
        text = self.brief
        if self.description != "":
            text += f"\n{self.description}"    
        for param in self.parameters.values():
            text += f"\n{param.get_comment_text()}"
        text += "\n@return Error_t An error is returned if\n"
        text += "- processing the message results in an error.\n"
        text += "Otherwise ERROR_NONE is returned.\n"
        return utils.text_to_comment(text)
    
    def _prototype(self) -> str:
        text = self._comment()
        text += f"Error_t {self.func_name}"
        text += self._params()
        text += ";\n\n"
        return text
    
    def _struct(self) -> str:
        if len(self.args) == 0:
            return ""
        text = f"struct {self.type}Args_{self.name}_s {'{'}\n"
        for arg in self.args.values():
            text += f"\t{arg.type} {arg.name};\n"
        text += "};\n\n"
        return text
    
    def _header(self) -> str:
        return ""

    def _body(self) -> str:
        return ""

    def get_name_as_comment(self) -> str:
        return f"/* {self.name} */\n\n"

    def get_header_text(self) -> str:
        text = self._prototype()
        text += self._struct()
        return text
    
    def get_source_text(self) -> str:
        text = self._body()
        return text

class RequestSender(Sender):
    def __init__(self, 
        name:str, 
        response:bool, 
        description:str="", 
        args:Dict[str, Parameter]={}, 
        message_id:bool=False, 
        ) -> None:
        self.response = response
        self.message_id = message_id

        type = "request"
        func_name = f"req_{name}"
        parameters = args.copy()
        if message_id:
            mid = Parameter(
                name="MessageID",
                type="MessageID_t",
                description="An ID to associate the message.")
            parameters["MessageID"] = mid
        if response:
            description += "\nResponse: Yes\n"
            response_handler = Parameter(
                name="ResponseHandler",
                type="MessageHandler_t",
                description="A pointer to the function\n" \
                "that will handle the response to this request.\n")
            kernel_id = Parameter(
                name="KernelID",
                type="KernelID_t",
                description="The ID of the kernel to which\n" \
                "the requesting module belongs.\n")
            parameters["ResponseHandler"] = response_handler
            parameters["KernelID"] = kernel_id
        else:
            description += "\nResponse: No\n"

        brief = f"@brief Send a message to request: {name}.\n"

        super().__init__(
            name,
            func_name,
            type,
            brief,
            description,
            parameters,
            args)

    def _header(self) -> str:
        text = "\n\tMessageHeader_t MessageHeader = {\n"
        text += f"\t\t.RequestID = {self.RID},\n"
        if self.response:
            text += "\t\t.ResponseHandler = ResponseHandler,\n"
            text += "\t\t.KernelID = KernelID,\n"
        if self.message_id:
            text += "\t\t.MessageID = MessageID,\n"
        text += "\t};\n"
        return text
    
    def _body(self) -> str:
        text = f"Error_t {self.func_name}"
        text += self._params()
        text += "\n{"
        text += self._header()
        text += "\n\treturn KernelSwitch_sendRequest(\n"
        text += "\t\t&MessageHeader,\n"
        if len(self.args) == 0:
            text += "\t\tNULL);\n}\n"
        else:
            text += f"\t\t&(struct requestArgs_{self.name}_s){'{'}\n"
            for arg in self.args.values():
                text += f"\t\t\t{arg.name},\n"
            text += "\t\t});\n}\n\n"
        return text
        
class ResponseSender(Sender):
    def __init__(self, 
        name:str,  
        description:str="", 
        args:Dict[str, Parameter]={},  
        ) -> None:
        type = "response"
        func_name = f"res_{name}"
        parameters = args.copy()
        request_header = Parameter(
            "RequestHeader",
            "MessageHeader_t*",
            "A pointer to the header of\n"\
            "the request message to which this response is sent."
        )
        parameters["RequestHeader"] = request_header
        brief = f"@brief Send a response to the request: {name}.\n"

        super().__init__(
            name,
            func_name,
            type,
            brief,
            description,
            parameters,
            args)
        
    def _body(self) -> str:
        text = f"Error_t {self.func_name}"
        text += self._params()
        text += "\n{"
        text += self._header()
        text += "\n\treturn KernelSwitch_sendResponse(\n"
        text += "\t\tRequestHeader,\n"
        if len(self.args) == 0:
            text += "\t\tNULL);\n}\n"
        else:
            text += f"\t\t&(struct responseArgs_{self.name}_s){'{'}\n"
            for arg in self.args.values():
                text += f"\t\t\t{arg.name},\n"
            text += "\t\t});\n}\n\n"
        return text
    
class Request():
    def __init__(self,
        name:str,
        request_description:str="",
        request_args:Dict[str, Parameter]={},
        has_message_id:bool=False,
        has_response:bool=False,
        response_description:str="",
        response_args:Dict[str, Parameter]={},
        ) -> None:
        self.name = name
        self.has_response = has_response
        self.has_message_id = has_message_id
        self.has_request_args = True if len(request_args) > 0 else False
        self.has_response_args = True if len(response_args) > 0 else False
        self.request_sender = RequestSender(
            name=name,
            response=has_response,
            description=request_description,
            args=request_args,
            message_id=has_message_id)
        if has_response:
            self.response_sender = ResponseSender(
                name=name,
                description=response_description,
                args=response_args)
        self.RID = f"RID_{name}"
        self.request_handlers:List[RequestHandler] = []
        self.response_handlers:List[ResponseHandler] = []
        self.used_by:Set[str] = set()
        self.interface = None

    def get_header_text(self):
        text = self.request_sender.get_name_as_comment()
        text += self.request_sender.get_header_text()
        if self.has_response:
            text += self.response_sender.get_header_text()
        return text

    def get_source_text(self):
        text = self.request_sender.get_source_text()
        if self.has_response:
            text += "\n"
            text += self.response_sender.get_source_text()
        return text
    
class Handler():
    def __init__(self,
        name:str,          
        func_name:str,
        brief:str,         
        type:str,
        has_args:bool,
        has_message_header:bool,
        description:str="",
        user_codes:Dict[str, UserCode]={},
        module_name:str=""
        ) -> None:
        self.name = name  
        self.func_name = func_name
        self.brief = brief
        self.type = type    
        self.has_args = has_args
        self.has_header = has_message_header
        if description[-1] != "\n":
            description += "\n"
        self.description = description
        id=f"{type.upper()} {utils.name_to_filename(name=name).replace('_', ' ').upper()}"
        self.user_code = user_codes[id] if id in user_codes else UserCode(identifier=id)
        self.user_code.indents = 1
        self.module_name = module_name

    def _comment(self):
        text = self.brief
        if self.description != "":
            text += f"\n{self.description}"
        if self.has_header:
            text += f"\n@param Header Pointer to the header of the {self.type} message.\n"    
        if self.has_args:
            text += f"\n@param Args Pointer to the {self.type} parameters.\n"
        return utils.text_to_comment(text)
    
    def get_prototype(self):
        text = self._comment()
        text += f"static void {self.func_name}("
        if self.has_args and self.has_header:
            text += "\n\tMessageHeader_t *Header,"
            text += f"\n\tstruct {self.type}Args_{self.name}_s *Args);\n\n"
        elif self.has_args and not self.has_header:
            text += "\n\tMessageHeader_t *Header,"
            text += f"\n\tstruct {self.type}Args_{self.name}_s *Args);\n\n"
        elif not self.has_args and self.has_header:
            text += "\n\tMessageHeader_t *Header);\n\n"
        elif not self.has_args and not self.has_header:
            text += "void);\n\n"
        return text
    
    def get_body(self):
        text = utils.text_to_comment(self.description)
        text += f"void {self.func_name}("
        if self.has_args and self.has_header:
            text += "\n\tMessageHeader_t *Header,"
            text += f"\n\tstruct {self.type}Args_{self.name}_s *Args)\n"
        elif self.has_args and not self.has_header:
            text += "\n\tMessageHeader_t *Header,"
            text += f"\n\tstruct {self.type}Args_{self.name}_s *Args)\n"
        elif not self.has_args and self.has_header:
            text += "\n\tMessageHeader_t *Header)\n"
        elif not self.has_args and not self.has_header:
            text += "void)\n"
        text += "{\n"
        text += self.user_code.get_text()
        text += "}\n\n"
        return text

class RequestHandler(Handler):
    def __init__(self,
        request:Request, 
        description:str="",
        user_codes:Dict[str, UserCode]={},
        module_name:str=""
        ) -> None:
        self.request = request
        self.RID = request.RID
        name = request.name
        func_name = f"handleRequest_{name}"
        brief = f"@brief Handle the request: {name}.\n"
        type = "request"
        has_args = True if request.has_request_args else False
        has_message_header = request.has_response or request.has_message_id       

        super().__init__(
            name=name,
            func_name=func_name,
            brief=brief,
            type=type,
            has_args=has_args,
            has_message_header=has_message_header,
            description=description,
            user_codes=user_codes,
            module_name=module_name)
        
class ResponseHandler(Handler):
    def __init__(self, 
        request:Request,
        description:str="",
        user_codes:Dict[str, UserCode]={},
        module_name:str=""
        ) -> None:
        self.request = request
        self.RID = request.RID
        name = request.name
        func_name = f"handleResponse_{name}"  
        brief = f"@brief Handle a response to the request: {name}.\n"
        type = "response"
        has_args = True if request.has_response_args else False
        has_message_header = True if request.has_message_id else False

        super().__init__(
            name=name,
            func_name=func_name,
            brief=brief,
            type=type,
            has_args=has_args,
            has_message_header=has_message_header,
            description=description,
            user_codes=user_codes,
            module_name=module_name)