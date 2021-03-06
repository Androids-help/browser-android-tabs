The pdf_compositor service should composite multiple raw pictures from different
frames into a complete one, then converts it into a pdf file within an isolated
sandboxed process. Currently, it has no compositing functionality, just convert
a set of raw pictures into a pdf file within the sandboxed process.

While the compositor creates single-page PDF objects it can optionally collect
those into a multi-page PDF document object.  Otherwise a multi-page PDF document
is made by sending an extra multi-page metafile which contains repeats of each
of the previously processed pages all in one larger message.

Message flow when interacting with the PDF compositor is as follows:

[![IPC flow for PDF compositor
usage](ipc_flow_diagram.png)](https://docs.google.com/drawings/d/1bhm3FfLaSL42f-zw41twnOGG0kdMKMuAGoEyGuGr6HQ)