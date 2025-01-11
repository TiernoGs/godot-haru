
#include "haru.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// Default page size
constexpr int32_t HPDF_DEFAULT_WIDTH = 1080;
constexpr int32_t HPDF_DEFAULT_HEIGHT = 1920;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Utils
String convert_string_to_filename(const String &path)
{
    String absolute_path;

    if (path.begins_with("res://"))
    {
        // Remove the "res://" part
        String relative_path = path.substr(6, path.length() - 6);

        // Get the base path of the project directory
        absolute_path = OS::get_singleton()->get_cache_dir().path_join(relative_path);
    }
    else if(path.begins_with("user://"))
    {
        // Remove the "user://" part
        String relative_path = path.substr(7, path.length() - 7);

        // Get the base path of the user data directory
        absolute_path = OS::get_singleton()->get_user_data_dir().path_join(relative_path);
    }
    else
    {
        // Return empty string
        return "";
    }

    String folder = absolute_path.get_base_dir();
    if(DirAccess::dir_exists_absolute(folder) == false)
    {
        DirAccess::make_dir_recursive_absolute(folder);
    }

    return absolute_path;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PDFExporter methods
PDFExporter::PDFExporter()
{
    UtilityFunctions::print("Loading PDF exporter...");

    // Create a new PDF
    pdf = HPDF_New(hdpf_error_handler, nullptr);
    if (!pdf)
    {
        ERR_FAIL_MSG("error: cannot create PdfDoc object");
        return;
    }

    page = HPDF_AddPage (pdf);
    set_resolution({HPDF_DEFAULT_WIDTH, HPDF_DEFAULT_HEIGHT});
}

PDFExporter::~PDFExporter()
{
    if(pdf)
    {
        HPDF_Free(pdf);
    }
}

void PDFExporter::set_resolution(const Vector2i& resolution)
{
    HPDF_Page_SetWidth(page, resolution.x);
    HPDF_Page_SetHeight(page, resolution.y);
}

Vector2i PDFExporter::get_resolution() const
{
    Vector2i resolution;
    resolution.x = HPDF_Page_GetWidth(page);
    resolution.y = HPDF_Page_GetHeight(page);
    return resolution;
}

void PDFExporter::export_recursive(Control* p_node)
{
    // TODO
    draw_rect({0,0}, {150, 150}, Color(0.2, 0.5, 0.8));
}

bool PDFExporter::export_pdf(Object *p_node, const String& filename)
{
    Control* node = Object::cast_to<Control>(p_node);
    if(node == nullptr)
    {
        printf("Expected a ControlNode");
        return false;
    }

    // Set PDF resolution according to parent node size
    set_resolution(node->get_size());

    // Export children
    export_recursive(node);

    String path = convert_string_to_filename(filename);
    printf("Saving pdf as '%s'\n", path.utf8().get_data());
    HPDF_STATUS status = HPDF_SaveToFile (pdf, path.utf8().get_data());

    return true;
}

void PDFExporter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("export", "p_node", "filename"), &PDFExporter::export_pdf);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Libharu specific
void PDFExporter::draw_text(const Vector2& position, const String& text)
{
    HPDF_Page_BeginText(page);
    HPDF_Page_MoveTextPos(page, position.x, position.y);
    HPDF_Page_ShowText(page, text.utf8().get_data());
    HPDF_Page_EndText(page);
}

void PDFExporter::draw_circle(const Vector2& position, double radius, const Color& color)
{

}

void PDFExporter::draw_rect(const Vector2& position, const Vector2& size, const Color& color)
{
    HPDF_Page_SetRGBFill(page, color.r, color.g, color.b);
    HPDF_Page_Rectangle(page, position.x, position.y, position.x + size.x, position.y + size.y);
}