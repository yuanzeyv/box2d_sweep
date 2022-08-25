#include "Library/Delabella/delabella.h"
#include "LuaBridge/DelabellaPort.h"
#include <string>
#include <iostream>
#include<fstream>//�����ļ���ͷ�ļ�
struct MyPoint
{
    double x;
    double y;
}; 
struct MyEdge
{
    int32_t a, b;
    MyEdge(int32_t a, int32_t b) : a(a), b(b) {};
};
void DelabellaPort::InitTable(sol::table solTable)
{
    auto DelabellaPortTable = solTable.new_usertype<DelabellaPort>("Delabella");
    DelabellaPortTable["Generate3DMap"] = [](std::string path, bool invert)->bool {//�ɹ�������
        std::vector<MyPoint> cloud;
        std::vector<MyEdge> force;
        FILE* f = fopen((path + ".txt").c_str(), "r");//��ȡһ���ļ����ļ���Ϣ 
        int r = 0, n = 0, c = 0;//r�����ȡ�Ĳ�����Ŀ  n����ǰ�ĵ������� c����ǰ�����Ʊ�
        r = fscanf(f, "%d %d", &n, &c);//��ȡ��ǰ���ı����� 
        for (int i = 0; n < 0 || i < n; i++) {
            MyPoint p;
            int n = fscanf(f, "%lf%*[,; \v\t]%lf%*[^\n]", &p.x, &p.y);//��ȡ���ݵ���X �� Y  
            cloud.push_back(p);
        }
        for (int i = 0; i < c; i++) {
            MyEdge e(0, 0);//һ�����������������
            r = fscanf(f, "%d %d", &e.a, &e.b);
            force.push_back(e);
        }
        fclose(f);//�ر��ļ�
        int32_t points = cloud.size();//��ȡ����ǰ�ĵ���  
        IDelaBella2<double, int32_t>* idb = IDelaBella2<double, int32_t>::Create();//����һ����صĽṹ��
        idb->SetErrLog([](void* stream, const char* fmt, ...) {
            va_list arg;
            va_start(arg, fmt);
            int ret = vfprintf((FILE*)stream, fmt, arg);
            va_end(arg);
            fflush((FILE*)stream);
            return ret;
            }
        , stdout);//���ó����ӡ��λ��  
        int32_t verts = idb->Triangulate(points, &cloud.data()->x, &cloud.data()->y, sizeof(MyPoint));
        int32_t tris_delabella = verts > 0 ? verts / 3 : 0;
        int32_t contour = idb->GetNumBoundaryVerts();
        int32_t non_contour = idb->GetNumInternalVerts();
        int32_t vert_num = contour + non_contour;
        int32_t voronoi_vertices = idb->GenVoronoiDiagramVerts(0, 0, 0);
        MyPoint* voronoi_vtx_buf = (MyPoint*)malloc((int32_t)voronoi_vertices * sizeof(MyPoint));
        if (!voronoi_vtx_buf)
            return false;
        idb->GenVoronoiDiagramVerts(&voronoi_vtx_buf->x, &voronoi_vtx_buf->y, sizeof(MyPoint));

        int32_t voronoi_closed_indices;
        int32_t voronoi_indices = idb->GenVoronoiDiagramPolys(0, 0, 0);
        int32_t* voronoi_idx_buf = (int32_t*)malloc(voronoi_indices * sizeof(int32_t));
        if (!voronoi_idx_buf)
            return false;
        idb->GenVoronoiDiagramPolys(voronoi_idx_buf, sizeof(int32_t), &voronoi_closed_indices);
        if (force.size() > 0)
        {
            idb->ConstrainEdges((int32_t)force.size(), &force.data()->a, &force.data()->b, (int)sizeof(MyEdge));;
            int32_t num_interior = idb->FloodFill(invert, 0);
            const IDelaBella2<double, int32_t>::Simplex** dela_polys = (const IDelaBella2<double, int32_t>::Simplex**)malloc(sizeof(const IDelaBella2<double, int32_t>::Simplex*) * (int32_t)num_interior);
            idb->FloodFill(invert, dela_polys);
            int index = 1;
            const IDelaBella2<double, int32_t>::Simplex* face = *dela_polys;
            std::ofstream ofs;						//����������
            ofs.open(path + ".obj", std::ios::out);		//��д�ķ�ʽ���ļ�
            while (face) {
                ofs << "v " << face->v[0]->x << " 0 " << face->v[0]->y << "\n";
                ofs << "v " << face->v[1]->x << " 0 " << face->v[1]->y << "\n";
                ofs << "v " << face->v[2]->x << " 0 " << face->v[2]->y << "\n";
                ofs << "f " << index << " " << (index + 1) << " " << (index + 2) << "\n";
                index = index + 3;
                face = face->next;
            }
            free(dela_polys);
            ofs.close();
            return  true;
        }
        return false;
    };
}
