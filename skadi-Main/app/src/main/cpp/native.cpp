//
// Created by andre on 1/21/21.
//
#include <string>
#include <vector>
#include "native.h"
#include "procmon.h"

hefesto::ProcMon procmon_obj;

extern "C"
JNIEXPORT jobject JNICALL Java_com_hefesto_skadi_NativeCall_00024Companion_getProcessInfo(JNIEnv *env, jobject this_obj)
{
    // create vector that will store process infos
    jclass vt_cls = env->FindClass("java/util/Vector");
    jmethodID init_id =  env->GetMethodID(vt_cls, "<init>", "()V");
    jmethodID add_meth = env->GetMethodID(vt_cls, "add", "(Ljava/lang/Object;)Z");
    jobject out_vt = env->NewObject(vt_cls, init_id);


    // for each inner object you have to create a object again
    jclass cls_pi = env->FindClass("com/hefesto/skadi/ProcessInfo");
    jmethodID ent_init = env->GetMethodID(cls_pi, "<init>", "()V");

    // get the field IDs
    jfieldID name_field = env->GetFieldID(cls_pi, "name", "Ljava/lang/String;");
    jfieldID owner_field = env->GetFieldID(cls_pi, "owner", "Ljava/lang/String;");
    jfieldID pid_field = env->GetFieldID(cls_pi, "pid", "I");
    jfieldID load_field = env->GetFieldID(cls_pi, "cpu_usage", "F");

    std::vector<ProcessInfo> proc_vec;
    proc_vec = procmon_obj.getData();

    jobjectArray pi_array = env->NewObjectArray(proc_vec.size(), cls_pi, 0);

    std::vector<ProcessInfo>::iterator cur_ite = proc_vec.begin();
    for(int i = 0; i < proc_vec.size(); i++)
    {
        // get the values
        jstring name = env->NewStringUTF(cur_ite->name.c_str());
        jstring owner = env->NewStringUTF(cur_ite->owner.c_str());
        jint pid = cur_ite->pid;
        jfloat cpu_usage = cur_ite->cpu_usage;

        jobject new_obj = env->NewObject(cls_pi, ent_init);

        // set the fields
        env->SetObjectField(new_obj, name_field, name);
        env->SetObjectField(new_obj, owner_field, owner);
        env->SetIntField(new_obj, pid_field, pid);
        env->SetFloatField(new_obj, load_field, cpu_usage);

        // insert the object to array
        //env->SetObjectArrayElement(pi_array, i, new_obj);
        env->CallBooleanMethod(out_vt, add_meth, new_obj);

        ++cur_ite;
    }

    return out_vt;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hefesto_skadi_NativeCall_00024Companion_refreshProcessList(JNIEnv *env, jobject thiz) {
    procmon_obj.refresh();
}

std::string getTemp()
{
    FILE *fp;
    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    char temp[5];
    fgets(temp, 5, fp);
    return temp;
}

//extern "C"
//JNIEXPORT jstring JNICALL Java_com_hefesto_procmon_ProcessActivity_getTemperature(JNIEnv* env, jobject jobject)
//{
//    std::string temp = getTemp();
//    return env->NewStringUTF(temp.c_str());
//}

extern "C"
JNIEXPORT jstring JNICALL Java_com_hefesto_skadi_NativeCall_00024Companion_getTemperature(JNIEnv* env, jobject jobject)
{
    std::string temp = getTemp();
    return env->NewStringUTF(temp.c_str());
}